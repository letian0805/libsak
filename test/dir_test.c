#include <stdio.h>
#include "sak_dir.h"

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("usage: dir_test [dir]\n");
        exit(1);
    }
    SakDir *dir = dir_open(argv[1]);
    if (dir == NULL){
        printf("dir path %s is wrong\n", argv[1]);
        exit(1);
    }
    SakDirItem item;
    while(dir_read(dir, &item) == 0){
        printf("find %s: %s\n", (item.type == SAK_FILE_DIR)?"dir":"file", item.name);
    }
    dir_close(dir);
    return 0;
}
