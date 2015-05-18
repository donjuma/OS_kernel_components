#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define MAX_SIZE  1024

void print_group(gid_t gid);
void print_perm(int perm);
void print_user(uid_t uid);

int main()
{
    char dirname[MAX_SIZE];
    char filename[MAX_SIZE];
    char buffer[MAX_SIZE];

    DIR *dirp;
    struct direct *dp;
    struct stat buf;
    struct stat *sp = &buf;
    int stat_status;

    strcpy(dirname, ".");

    dirp = opendir(dirname);
    if (dirp == NULL) {
        fprintf(stderr, "No such directory as %s\n", dirname);
        exit(1);
    }
    while ((dp = readdir(dirp)) != NULL)
    {
        sprintf(filename, "/home/castor1/hamilton/courses/330/%s/%s",
            dirname, dp->d_name);
        sprintf(filename, "%s",
            dp->d_name);
        stat_status = stat(filename, sp);
        if (stat_status == -1)
        {
            perror("lsl");
        }

        /*
            printf("Inode = %d\n",  sp->st_ino);
            printf("Mode = %o\n",  sp->st_mode);
        */
        print_perm(sp->st_mode);
        printf(" %4d ", sp->st_nlink);
        /*
            printf("uid = %d\n", sp->st_uid);
        */
        print_user(sp->st_uid);
        printf(" ");
        print_group(sp->st_gid);
        printf("%8li ", (long) sp->st_size);
        strcpy(buffer, ctime(&(sp->st_mtime)));
        buffer[16] = '\0';  /* remove newline */
        printf(&buffer[4]);
        printf(" %s\n", dp->d_name);
    }
    closedir(dirp);
    return 0;
}
void print_perm(int perm)
{
    printf("-");
    if ((perm & S_IRUSR) == S_IRUSR) printf("r"); else printf("-");
    if ((perm & S_IWUSR) == S_IWUSR) printf("w"); else printf("-");
    if ((perm & S_IXUSR) == S_IXUSR) printf("x"); else printf("-");
    if ((perm & S_IRGRP) == S_IRGRP) printf("r"); else printf("-");
    if ((perm & S_IWGRP) == S_IWGRP) printf("w"); else printf("-");
    if ((perm & S_IXGRP) == S_IXGRP) printf("x"); else printf("-");
    if ((perm & S_IROTH) == S_IROTH) printf("r"); else printf("-");
    if ((perm & S_IWOTH) == S_IWOTH) printf("w"); else printf("-");
    if ((perm & S_IXOTH) == S_IXOTH) printf("x"); else printf("-");
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
}
void print_user(uid_t uid)
{
    struct passwd *p;
    p = getpwuid(uid);
    printf("%-8s", p->pw_name);
}
void print_group(gid_t gid)
{

    struct group *g;
    g = getgrgid(gid);
    printf("%-8s", g->gr_name);
}
