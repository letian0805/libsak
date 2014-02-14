#ifndef __LOG_H
#define __LOG_H

enum{
    LOG_OFF = 0,
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_ALL,
}LogLevel;

char *getpname(void);

int log_init(int level, const char *log_file);

void log_print(int level, const char *src_file, const char *src_func, int src_line, const char *format, ...);

#define DEBUG(__format, __args...)  log_print(LOG_DEBUG,  __FILE__, __func__, __LINE__, __format, ##__args)
#define INFO(__format, __args...)   log_print(LOG_INFO,   __FILE__, __func__, __LINE__, __format, ##__args)
#define WARN(__format, __args...)   log_print(LOG_WARN,   __FILE__, __func__, __LINE__, __format, ##__args)
#define ERROR(__format, __args...)  log_print(LOG_ERROR,  __FILE__, __func__, __LINE__, __format, ##__args)
#define FATAL(__format, __args...)  log_print(LOG_FATAL,  __FILE__, __func__, __LINE__, __format, ##__args)

#endif //!__LOG_H
