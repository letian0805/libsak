#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "log.h"

static int log_level = LOG_DEBUG;
static FILE *log_file = NULL;

char *gettimestr(char *tmstr)
{
    static char time_str[32] = {0};
    struct tm vntm;
    time_t tmt = time(NULL);

    if (tmstr == NULL){
        tmstr = time_str;
    }
    localtime_r(&tmt, &vntm);
    sprintf(tmstr, "%04d-%02d-%02d %02d:%02d:%02d", 
            vntm.tm_year + 1900, vntm.tm_mon + 1, vntm.tm_mday, 
            vntm.tm_hour, vntm.tm_min, vntm.tm_sec);

    return tmstr;
}

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
    pname = strrchr(buf, '/');
    pname = pname ? pname + 1 : buf;

    strcpy(prog_name, pname);
    fclose(fp);

    return prog_name;
}

static const char *level_string(int level)
{
    char *level_str[LOG_ALL] = {
        [LOG_OFF] = "",
        [LOG_DEBUG] = "DBG",
        [LOG_INFO] = "INF",
        [LOG_WARN] = "WRN",
        [LOG_ERROR] = "ERR",
        [LOG_FATAL] = "FTL",
    };
    if (level > LOG_OFF && level < LOG_ALL){
        return level_str[level];
    }
    return "";
}

int log_init(int level, const char *file)
{
    int ret = 0;
    log_level = level;
    if ( !file || log_file){
        return ret;
    }
    FILE *fp = fopen(file, "a+");
    if (fp){
        log_file = fp;
    }else{
        ret = errno;
        ERROR("open %s failed: %s", file, strerror(errno));
    }
    return ret;
}

void log_print(int level, const char *file, const char *func, int line, const char *format, ...)
{
    if (log_level < level){
        return;
    }
    char fmt[1024] = {0};
    char timestr[32] = {0};
    FILE *fp = (log_file)?log_file:stdout;
    char *f = strrchr(file, '/');
    if (f){
        file = f + 1;
    }
    va_list args;

    if (log_level >= LOG_DEBUG){
        sprintf(fmt, "[%s][%s][%s|%s|%s()|%d]: %s\n", 
                    gettimestr(timestr), level_string(level), 
                    getpname(), file, func, line, format);
    }else{
        sprintf(fmt, "[%s][%s][%s]: %s\n", gettimestr(timestr), getpname(), 
                    level_string(level), format);
    }

    va_start(args, format);
    vfprintf(fp, fmt, args);
    if (level <= LOG_ERROR){
       vfprintf(stderr, fmt, args);
    }
    va_end(args);

    if (level == LOG_FATAL){
        exit(-1);
    }
}
