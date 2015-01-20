#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "sak.h"

static SakLogLevel log_level = SAK_LOG_DEBUG;
static FILE *log_file = NULL;

char *gettimestr(char *tmstr)
{
    static char time_str[32] = {0};
    struct tm *pvntm = NULL;
    struct tm vntm;
    time_t tmt = time(NULL);

    if (tmstr == NULL){
        tmstr = time_str;
    }
    pvntm = localtime(&tmt);
    vntm = *pvntm;
    sprintf(tmstr, "%04d-%02d-%02d %02d:%02d:%02d", 
            vntm.tm_year + 1900, vntm.tm_mon + 1, vntm.tm_mday, 
            vntm.tm_hour, vntm.tm_min, vntm.tm_sec);

    return tmstr;
}

static const char *level_string(int level)
{
    char *level_str[SAK_LOG_ALL] = {
        [SAK_LOG_OFF] = "",
        [SAK_LOG_DEBUG] = "DBG",
        [SAK_LOG_INFO] = "INF",
        [SAK_LOG_WARN] = "WRN",
        [SAK_LOG_ERROR] = "ERR",
        [SAK_LOG_FATAL] = "FTL",
    };
    if (level > SAK_LOG_OFF && level < SAK_LOG_ALL){
        return level_str[level];
    }
    return "";
}

int sak_log_init(SakLogLevel level, const char *file)
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
        SAK_ERROR("open %s failed: %s", file, strerror(errno));
    }
    return ret;
}

void sak_log_print(SakLogLevel level, const char *src_file, const char *src_func, int src_line, const char *format, ...)
{
    if (log_level < level){
        return;
    }
    char fmt[1024] = {0};
    char timestr[32] = {0};
    FILE *fp = (log_file)?log_file:stdout;
    char *f = strrchr(src_file, '/');
    if (f){
        src_file = f + 1;
    }
    va_list args;

    if (log_level >= SAK_LOG_DEBUG){
        sprintf(fmt, "[%s][%s][%s|%s|%s()|%d]: %s\n", 
                    gettimestr(timestr), level_string(level), 
                    sak_progname(), src_file, src_func, src_line, format);
    }else{
        sprintf(fmt, "[%s][%s][%s]: %s\n", gettimestr(timestr), sak_progname(), 
                    level_string(level), format);
    }

    va_start(args, format);
    vfprintf(fp, fmt, args);
    if (level <= SAK_LOG_ERROR){
       vfprintf(stderr, fmt, args);
    }
    va_end(args);

    if (level == SAK_LOG_FATAL){
        exit(-1);
    }
}
