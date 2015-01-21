#ifndef __SAK_DL_H

#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#define __SAK_DL_H

typedef struct SakDl SakDl;

SakDl *sak_dl_open(const char *dlfile);

void *sak_dl_getsymbol(SakDl *dl, const char *symbol_name);

void sak_dl_close(SakDl *dl);

#endif // __SAK_DL_H
