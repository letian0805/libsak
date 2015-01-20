#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *getpname(void)
{
    static char prog_name[1024] = {0};
    if (prog_name[0]){
        return prog_name;
    }

    char *pname = NULL;
    char buf[1024] = {0};

    int ret = readlink("/proc/self/exe", buf, 1023);
    if (ret <= 0 || ret >=1024){
        return "";
    }

    pname = strrchr(buf, '/');
    pname = pname ? pname + 1 : buf;

    strcpy(prog_name, pname);

    return prog_name;
}

