#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
///#include "homebrew.h"

#define PROMPT "$> "
#define ONE_KB 1024
char *BIN_LOCATION;

char *homebrew_names[] = {
    "exit",
    "cd",
    "pwd",
    "listf",
    "calc"
};

int homebrew_exit(char **arguments){
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

int homebrew_cd(char **arguments){
    char *dir = arguments[1];
    char buffer[BUFSIZ];
    char *cwd;

    if (dir != NULL){
        if (chdir(dir) != 0){
            printf("No such directory: %s\n", dir);
        }else{
            cwd = getcwd(buffer, sizeof(buffer));
            printf("cwd changed to %s\n", cwd);
        }
    }else{
        //TODO If no arg given, treat like bash and change to /home/USER...
        printf("No argument given to cd!\n");
    }
    return 1;
}

int homebrew_pwd(char **arguments){
    char buffer[BUFSIZ];
    char *cwd;

    cwd = getcwd(buffer, sizeof(buffer));
    printf("%s ", buffer);
    return 1;
}

int homebrew_listf(char **arguments){
    pid_t pid;
    pid_t wpid;
    int status;
    char path[1024];
    strcpy(path, BIN_LOCATION);
    strcat(path, "/listf");

    pid = fork();
    if (pid < 0) {
        perror("Error forking child process!\n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(path, arguments) == -1) {
            // COMMENTED OUT SO THEY DONT KEEP PRINTING FOR COMANDS NOT FOUND
            //perror("Command Failed after forking process for execution!");
            printf("No command \"%s\" found.\n", arguments[0]);
            perror("");
        }
        exit(1);
    }
    else {
        //Code Segment from "wait - The Open Group" (opengroup.org)
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int homebrew_calc(char **arguments){
    pid_t pid;
    pid_t wpid;
    int status;
    char path[1024];
    strcpy(path, BIN_LOCATION);
    strcat(path, "/calc");

    pid = fork();
    if (pid < 0) {
        perror("Error forking child process!\n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(path, arguments) == -1) {
            // COMMENTED OUT SO THEY DONT KEEP PRINTING FOR COMANDS NOT FOUND
            //perror("Command Failed after forking process for execution!");
            printf("No command \"%s\" found.\n", arguments[0]);
            perror("");
        }
        exit(1);
    }
    else {
        //Code Segment from "wait - The Open Group" (opengroup.org)
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


int terminate(char **args){
    exit(0);
    return 0;
}

int (*homebrew_functions[]) (char **) = {
    &homebrew_exit,
    &homebrew_cd,
    &homebrew_pwd,
    &homebrew_listf,
    &homebrew_calc
};

int execute(char **args){
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();
    if (pid < 0) {
        perror("Error forking child process!\n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            // COMMENTED OUT SO THEY DONT KEEP PRINTING FOR COMANDS NOT FOUND
            //perror("Command Failed after forking process for execution!");
            printf("No command \"%s\" found.\n", args[0]);
            perror("");
        }
        exit(1);
    }
    else {
        //Code Segment from "wait - The Open Group" (opengroup.org)
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int process_cmd(char **arguments){
    int i;
    for (i = 0; i < 5; i++) {
        if (strcmp(arguments[0], homebrew_names[i]) == 0) {
            return (*homebrew_functions[i])(arguments);
        }
    }
    return execute(arguments);
}

char *get_cmd(void){
    int size_of_buffer = ONE_KB;
    int i = 0;
    int input;
    char *cmd = malloc(sizeof(char) * size_of_buffer);
    if (!cmd) {
        perror("Unable to allocate space for user command\n");
        exit(1);
    }
    while(1) {
        input = getchar();

        //If user hits ENTER, stop reading and process their command... (NOTE: \n == [ENTER])
        if (input == '\n') {
            cmd[i] = '\0';
            return cmd;
        }
        //Terminate on Ctrl-D as per assignment instructions. (NOTE: Ctrl-D implies EOF for terminal input)
        else if(input == EOF){
            exit(0);
        }
        else {
            cmd[i] = input;
        }
        i++;

        //Use same logic from process_args below for large user input.
        if (i >= size_of_buffer) {
            size_of_buffer += ONE_KB;
            cmd = realloc(cmd, size_of_buffer);
            if (!cmd) {
                perror("Unable to allocate space for user command\n");
                exit(1);
            }
        }
    }
}

char **process_args(char *command){
    int size_of_buffer = ONE_KB;
    int i = 0;
    char *arg;
    char delimiters[] = " ";
    char **arguments = malloc(ONE_KB * sizeof(char*));
    if (!arguments) {
        perror("Unable to allocate space for arguments\n");
        exit(1);
    }
    arg = strtok(command, delimiters);
    while (arg != NULL) {
        arguments[i] = arg;
        i++;
        if (i >= size_of_buffer) {
            size_of_buffer += ONE_KB;
            arguments = realloc(arguments, size_of_buffer * sizeof(char*));
            if (!arguments) {
                perror("Unable to allocate space for arguments\n");
                exit(1);
            }
        }
        arg = strtok(NULL, delimiters);
    }
    arguments[i] = NULL;
    return arguments;
}

static void prompt(){
    printf(PROMPT);
    fflush(stdout);
}

void shell_init(int argc, char **argv){
    char path[1024];
    char dest[1024];
    pid_t pid = getpid();
    sprintf(path, "/proc/%d/exe", pid);
    readlink(path, dest, 1024);
    dest[strlen(dest)-6] = 0;
    BIN_LOCATION = dest;
}


int main(int argc, char **argv){
    char *command;
    char **arguments;

    shell_init(argc, argv);
    while(1){
        prompt();
        command = get_cmd();
        arguments = process_args(command);
        if (arguments[0] != NULL){
            if (process_cmd(arguments) < 0){
                exit(0);
                break;
            }
        }
    }
    return EXIT_SUCCESS;
}
