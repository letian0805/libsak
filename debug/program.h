#ifndef __PROGRAM_H
#define __PROGRAM_H
#include <time.h>

int program_init(int *argc, char ***argv, bool hide_args);

int program_time(struct tm *ptm);

int program_start_time(struct tm *ptm);

char *program_name(char *namestr);

int program_exit(void);

#endif //!__PROGRAM_H
