#include "program.h"

typedef struct{
    bool inited;
    char name[1024];
    pid_t pid;
    int argc;
    char **argv;
    time_t start_time;
    struct tm start_local_time;
}ProgramInfo;

static ProgramInfo g_pinfo;

int program_init(int *pargc, char ***pargv, bool hide_args)
{
    if (pargc && pargv){
        if (!hide_args){
            g_pinfo.argc = *pargc;
            g_pinfo.argv = *pargv;
        }else{

        }
    }
}

int program_time(struct tm *ptm)
{

}

int program_start_time(struct tm *ptm)
{

}

const char *program_name(void)
{
    return (const char *)g_pinfo.name;
}

int program_exit(void)
{

}

