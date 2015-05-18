#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

char permissions[30];
int lflag = 0;
int mflag = 0;
int aflag = 0;
int cflag = 0;


/*
   S_IWUSR   00200   Write by owner.
   S_IXUSR   00100   Execute (search if a directory) by owner.

   S_IRWXG   00070   Read, write, execute by group.
   S_IRGRP   00040   Read by group.
   S_IWGRP   00020   Write by group.
   S_IXGRP   00010   Execute by group.
   S_IRWXO   00007   Read, write, execute (search) by others.
   S_IROTH   00004   Read by others.
   S_IWOTH   00002   Write by others
   S_IXOTH   00001   Execute by others.
*/

char *get_perms(mode_t mode)
{
    char ftype = '?';

    if (S_ISREG(mode)) ftype = '-';
    if (S_ISLNK(mode)) ftype = 'l';
    if (S_ISDIR(mode)) ftype = 'd';
    if (S_ISBLK(mode)) ftype = 'b';
    if (S_ISCHR(mode)) ftype = 'c';
    if (S_ISFIFO(mode)) ftype = '|';

    sprintf(permissions, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
            mode & S_IRUSR ? 'r' : '-',
            mode & S_IWUSR ? 'w' : '-',
            mode & S_IXUSR ? 'x' : '-',
            mode & S_IRGRP ? 'r' : '-',
            mode & S_IWGRP ? 'w' : '-',
            mode & S_IXGRP ? 'x' : '-',
            mode & S_IROTH ? 'r' : '-',
            mode & S_IWOTH ? 'w' : '-',
            mode & S_IXOTH ? 'x' : '-',
            mode & S_ISUID ? 'U' : '-',
            mode & S_ISGID ? 'G' : '-',
            mode & S_ISVTX ? 'S' : '-');

    return (char *)permissions;
}

char pathname[MAXPATHLEN];
char cwd[1024];

int print_files(){
    int files;
    int i;
    double digits;
    struct direct **entries;
    struct stat meta;
    char access_date[1024];
    char modification_date[1024];
    char creation_date[1024];
    struct passwd passwd;
    struct passwd *passwd_ptr;
    struct group grp;
    struct group *grpt;
    struct tm time;
    char buffer[1024];
    int max_size = 1;
    const char *hidden = ".";
    const char *filename;
    int hidden_count = 0;
    int filesize;

    getcwd(pathname, sizeof(pathname));
    files = scandir(pathname, &entries, NULL, alphasort);

    if(files > 0){

        for (i=0; i < files; i++){
            if (stat(entries[i]->d_name, &meta) == 0){
                //Obtain the maximum digits needed for file size...
                digits = floor(log10(abs(meta.st_size))) + 1;
                max_size = MAX(max_size, (int)digits);
                if (!strncmp(entries[i]->d_name, hidden, strlen(hidden))){
                    hidden_count++;
                }

            }
        }
        printf("%d entries found\n",files-hidden_count);
        for (i=0; i < files; i++){
            if (stat(entries[i]->d_name, &meta) == 0){

                if (!strncmp(entries[i]->d_name, hidden, strlen(hidden))){
                    continue;
                }

                if (!(lflag)){
                    printf("%s ", entries[i]->d_name);
                    continue;
                }

                printf("%10.10s %zu", get_perms(meta.st_mode), meta.st_nlink);
                //printf(" %zu", meta.st_nlink);

                if (!getpwuid_r(meta.st_uid, &passwd, buffer, sizeof(buffer), &passwd_ptr)){
                    printf(" %s", passwd.pw_name);
                }
                else{
                    printf(" %d", meta.st_uid);
                }

                if (!getgrgid_r (meta.st_gid, &grp, buffer, sizeof(buffer), &grpt)){
                    printf(" %s", grp.gr_name);
                }
                else{
                    printf(" %d", meta.st_gid);
                }
                filesize = (int)meta.st_size;
                if (S_ISDIR(meta.st_mode)) filesize = 4096;
                printf(" %*d", max_size, filesize);

                if ((mflag) || (!(cflag + aflag))){
                    //Extract time out of the time struct
                    localtime_r(&meta.st_mtime, &time);
                    strftime(modification_date, sizeof(modification_date), "M%D-%H:%M", &time);
                    printf(" %s", modification_date);
                }
                if (aflag){
                    //Extract time out of the time struct
                    localtime_r(&meta.st_atime, &time);
                    strftime(access_date, sizeof(access_date), "A%D-%H:%M", &time);
                    printf(" %s", access_date);
                }
                if (cflag){
                    //Extract time out of the time struct
                    localtime_r(&meta.st_ctime, &time);
                    strftime(creation_date, sizeof(creation_date), "C%D-%H:%M", &time);
                    printf(" %s", creation_date);
                }
                printf(" %s\n", entries[i]->d_name);
                //printf(" %s %s %s %s\n", modification_date, access_date, creation_date, entries[i]->d_name);
            }

            free (entries[i]);
        }
        if (!(lflag)){
            printf("\n");
        }
        free(entries);
    }
}

int main(int argc, char **argv){
    int input;
    int i;
    char *dirpath;
    char buffer[BUFSIZ];
    int dir = 0;
    opterr = 0;
    getcwd(cwd, sizeof(cwd));

    while ((input = getopt(argc, argv, "lmac")) != -1){
        switch (input){
            case 'l':
                lflag = 1;
                break;
            case 'm':
                mflag = 1;
                break;
            case 'a':
                aflag = 1;
                break;
            case 'c':
                cflag = 1;
                break;
            case '?':
                break;
            default:
                exit(0);
        }
    }

    for (i = optind; i < argc; i++){
        dir++;
        dirpath = argv[i];
        if (chdir(dirpath) != 0){
            printf ("Directory Not Found! %s\n", argv[i]);
            abort();
        }
        getcwd(pathname, sizeof(pathname));
        print_files();
        if (chdir(cwd) != 0){
            perror("Fatal error in changing directories!\n");
            abort();
        }
    }

    if (dir == 0){
        print_files();
    }
  return 0;
}


