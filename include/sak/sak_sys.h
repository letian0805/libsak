#ifndef __SAK_SYS_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long ull;

typedef unsigned long long SakTime;

char *sak_progname(void);

int sak_progid(void);

#ifdef __cplusplus
}
#endif

#endif //__SAK_SYS_H
