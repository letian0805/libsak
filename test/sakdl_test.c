#include <stdio.h>
#include <sak.h>

int main(int argc, char *argv[])
{
    if (argc < 3){
        printf("usage: sakdl_test [dlfilename] [symbolname]\n");
        return -1;
    }
    SakDl *dl = sak_dl_open(argv[1]);
    if (dl == NULL){
        printf("sak_dl_open(%s) failed!\n", argv[1]);
        return -1;
    }
    void *func = sak_dl_getsymbol(dl, argv[2]);
    if (func == NULL){
        printf("sak_dl_getsymbol can't get symbol \"%s\"!\n", argv[2]);
    }else{
        printf("sak_dl_getsymbol get symbol \"%s\" at address %p\n", argv[2], func);
    }
    sak_dl_close(dl);
    return 0;
}
