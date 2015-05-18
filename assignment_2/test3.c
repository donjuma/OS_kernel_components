#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>




/* split string and append tokens to 'res' */
int main(int argc, char *argv[], char *envp[]){
    //char str[]= "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games";
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error!");
    }
    printf("%s\n", cwd);
    char * str = getenv("PATH");
    printf("%s\n", str);
    strcat(str, ":");
    strcat(str, cwd);
    printf("%s\n", str);

    char ** res  = NULL;
    char *  p    = strtok (str, ":");
    int n_spaces = 0, i;

    while (p) {
      res = realloc (res, sizeof (char*) * ++n_spaces);

      if (res == NULL)
        exit (-1); /* memory allocation failed */

      res[n_spaces-1] = p;

      p = strtok (NULL, ":");
    }

    /* realloc one extra element for the last NULL */

    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;

    /* print the result */

    for (i = 0; i < (n_spaces+1); ++i)
      printf ("res[%d] = %s\n", i, res[i]);

    /* free the memory allocated */

    free (res);
}
