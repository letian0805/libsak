#include <stdio.h>
#include <string.h>
#define _GNU_SOURCE
#include <unistd.h>
int less20 = 0;
int less10 = 0;
int symore50 = 0;

int check_cpu(char *buf)
{
    if (strstr(buf, "Cpu")==NULL){
        return 0;
    }
    int update = 0;
    char *idstr = strstr(buf, "id");
    char *systr = strstr(buf, "sy");
    char *idstart = idstr;
    char *systart = systr;
    while(*idstart != ' '){
        idstart--;
    }
    while(*systart != ' '){
        systart--;
    }
    float rate = 0;
    sscanf(idstart, "%f", &rate);
    if (rate < 10.0){
        less10++;
        update = 1;
    }
    if (rate < 20.0){
        less20++;
        update = 1;
    }
    sscanf(systart, "%f", &rate);
    if (rate > 50){
        symore50++;
        update = 1;
    }
    printf("%%id less than 20.0%% count: %d\n", less20);
    printf("%%id less than 10.0%% count: %d\n", less10);
    printf("%%sy more than 50.0%% count: %d\n", symore50);

    return 0;
}

int main(void)
{
    FILE *fp = popen("top -d 1", "r");
    char buf[4096] = {0};
    int ret = 0;
    while(1){
        char *data = buf;
        while(fread(data, 1, 1, fp)){
            if (*data == '\n'){
                *(data + 1) = 0;
                check_cpu(buf);
                break;
            }
            data++;
        }
        memset(buf, 0, 4096);
    }

    fclose(fp);

    return 0;
}
