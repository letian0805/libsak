#include <sak.h>

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("usage: plugin_test [file/dir]\n");
        return -1;
    }
    if (sak_plugin_load(argv[1], 0) < 0){
        if (sak_plugin_loadall(argv[1], 0) < 0){
            printf("can't load plugin in path \"%s\"\n", argv[1]);
            return -1;
        }
    }
    return 0;
}
