#ifndef __SAK_TRACE_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

int sak_trace_init(const char *logdir);

#ifdef __cplusplus
}
#endif

#endif //!__SAK_TRACE_H
