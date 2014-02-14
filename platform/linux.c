#include <stdlib.h>
#include <string.h>

char *getpname(void)
{
    static char prog_name[1024] = {0};
    if (prog_name[0]){
        return prog_name;
    }

    char *pname = NULL;
    pid_t pid = getpid();
    char fname[1024] = {0};
    char buf[1024] = {0};

    sprintf(fname, "/proc/%d/cmdline", pid);
    FILE *fp = fopen(fname, "r");
    if (!fp){
        perror("fopen");
        return "";
    }
    fgets(buf, 1023, fp);
    fclose(fp);
    pname = strrchr(buf, '/');
    pname = pname ? pname + 1 : buf;

    strcpy(prog_name, pname);

    return prog_name;
}

