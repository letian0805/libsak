#ifndef __SAK_LOG_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_LOG_H
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    SAK_LOG_OFF = 0,
    SAK_LOG_FATAL,
    SAK_LOG_ERROR,
    SAK_LOG_WARN,
    SAK_LOG_INFO,
    SAK_LOG_DEBUG,
    SAK_LOG_ALL,
}SakLogLevel;

int sak_log_init(SakLogLevel level, const char *log_file);

void sak_log_print(SakLogLevel level, const char *src_file, const char *src_func, int src_line, const char *format, ...);

#define SAK_DEBUG(__format, ...)  sak_log_print(SAK_LOG_DEBUG,  __FILE__, __func__, __LINE__, __format, ##__VA_ARGS__)
#define SAK_INFO(__format, ...)   sak_log_print(SAK_LOG_INFO,   __FILE__, __func__, __LINE__, __format, ##__VA_ARGS__)
#define SAK_WARN(__format, ...)   sak_log_print(SAK_LOG_WARN,   __FILE__, __func__, __LINE__, __format, ##__VA_ARGS__)
#define SAK_ERROR(__format, ...)  sak_log_print(SAK_LOG_ERROR,  __FILE__, __func__, __LINE__, __format, ##__VA_ARGS__)
#define SAK_FATAL(__format, ...)  sak_log_print(SAK_LOG_FATAL,  __FILE__, __func__, __LINE__, __format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif //!__SAK_LOG_H
