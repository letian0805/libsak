#include <stdio.h>
#include <stdlib.h>
#include "sak.h"

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("usage: sak_dir_test [dir]\n");
        exit(1);
    }
    SakDir *dir = sak_dir_open(argv[1]);
    if (dir == NULL){
        printf("dir path %s is wrong\n", argv[1]);
        exit(1);
    }
    SakDirItem item;
    while(sak_dir_read(dir, &item) == 0){
        printf("find %s: %s\n", (item.type == SAK_FILE_DIR)?"dir":"file", item.name);
    }
    sak_dir_close(dir);
    return 0;
}
