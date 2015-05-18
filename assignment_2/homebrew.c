#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int (*homebrew_functions[]) (char **) = {
    &homebrew_exit,
    &homebrew_cd,
    &homebrew_pwd,
    &homebrew_listf,
    &homebrew_calc
};

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
    char dir = arguments[1];

    if (dir != NULL){
        if (chdir(dir) != 0){
            printf("No such directory: %s\n", dir);
        }else{
            printf("cwd changed to %s\n", dir);
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
    return 1;
}

int homebrew_calc(char **arguments){
    return 1;
}
