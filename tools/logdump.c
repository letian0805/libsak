#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static FILE *log_file = NULL;
static FILE *dump_file = NULL;

static bool check_prog(char *log, char *prog)
{
    if (prog == NULL){
        return true;
    }

    if (strncmp(log + 27, prog, strlen(prog))){
        return false;
    }

    int pos = 27 + strlen(prog);
    if (log[pos]!=']' && log[pos]!='|'){
        return false;
    }

    return true;
}

static bool check_date(char *log, char *date)
{
    if (date == NULL){
        return true;
    }

    if (strncmp(log + 1, date, strlen(date))){
        return false;
    }

    return true;
}

static bool check_level(char *log, char *level)
{
    if (level == NULL){
        return true;
    }

    if (strncmp(log + 22, level, strlen(level))){
        return false;
    }

    return true;
}

static bool check_log(char *log, char *date, char *prog, char *level)
{
    if (strlen(log)<30){
        return false;
    }
    if (log[0]!='[' || log[20]!=']' || 
        log[21]!='[' || log[25]!=']' || 
        log[26]!='['){
        return false;
    }
    if (!check_prog(log, prog) || !check_date(log, date) || !check_level(log, level)){
        return false;
    }
    return true;
}

static void dump_log(char *date, char *prog, char *level)
{
    char log[1024] = {0};
    while(fgets(log, 1023, log_file)){
        if (check_log(log, date, prog, level)){
            fputs(log, dump_file);
        }
    }
}

static void usage(void)
{
    printf("Usage: logdump optstring parameters\n");
    printf("  -i, --input=filename          file name of log\n");
    printf("  -o, --output=filename         file name for output\n");
    printf("  -p, --prog=progname           program name of who print the log\n");
    printf("  -l, --level=levelstring       level string of the log, the value is DBG INF WRN ERR FTL\n");
    printf("  -d, --date=datestring         date string of the log, the format is like 2014-02-07\n");
    printf("  -h, --help                    get help information\n");
}

static bool check_level_str(char *level)
{
    if (level == NULL){
        return true;
    }
    char *levels[] = {"DBG", "INF", "WRN", "ERR", "FTL", NULL};
    int i;
    for (i = 0; levels[i]; i++){
        if (strcmp(level, levels[i]) == 0){
            return true;
        }
    }
    return false;
}

static bool check_date_format(char *date)
{
    if (date == NULL){
        return true;
    }
    if (strlen(date) != 10 || date[4]!='-' || date[7]!='-'){
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    char *log_file_name = NULL;
    char *dump_file_name = NULL;
    char *prog_name = NULL;
    char *level_str = NULL;
    char *date_str = NULL;

    struct option opts[] = {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"prog", required_argument, NULL, 'p'},
        {"level", required_argument, NULL, 'l'},
        {"date", required_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0},
    };
    int optret = 0;
    int index = 0;
    while((optret = getopt_long(argc, argv, "i:o:p:l:d:h", opts, &index)) > 0){
        switch(optret){
            case 'i':
                log_file_name = optarg;
                break;
            case 'o':
                dump_file_name = optarg;
                break;
            case 'p':
                prog_name = optarg;
                break;
            case 'l':
                if (!check_level_str(optarg)){
                    goto WRONG_OPTION;
                }
                level_str = optarg;
                break;
            case 'd':
                if (!check_date_format(optarg)){
                    goto WRONG_OPTION;
                }
                date_str = optarg;
                break;
            case 'h':
                usage();
                return 0;
            default:
                goto WRONG_OPTION;
        }
    }
    if (!check_date_format(date_str) || !check_level_str(level_str)){
        goto WRONG_OPTION;
    }
    if (log_file_name){
        if ( (log_file = fopen(log_file_name, "r")) == NULL){
            printf("open %s failed: %s\n", log_file_name, strerror(errno));
            return -1;
        }
    }else{
        log_file = stdin;
    }

    if (dump_file_name){
        if ( (dump_file = fopen(dump_file_name, "a+")) == NULL){
            printf("open %s failed: %s\n", log_file_name, strerror(errno));
            return -1;
        }
    }else{
        dump_file = stdout;
    }
    dump_log(date_str, prog_name, level_str);

    return 0;
WRONG_OPTION:
    usage();
    return -1;
}
