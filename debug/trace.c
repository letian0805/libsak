#define _GNU_SOURCE
#include <dlfcn.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>
#include <execinfo.h>
#include <ucontext.h>
#include "platform.h"

#include "trace.h"

static char log_file[1024] = {0};
static int pipe_fd[2] = {-1,-1};
static void print_trace(const char *format, ...);
static pthread_mutex_t trace_lock;

static char *translate_signal(int sig)
{
#define SIG_CASE(_s) \
    case _s: \
        return #_s

    switch(sig)
    {
        SIG_CASE(SIGSEGV);
        SIG_CASE(SIGBUS);
        SIG_CASE(SIGABRT);
        default:
            break;
    }

    return "unknown";
}

static void print_trace(const char *format, ...)
{
    va_list args;
    FILE *fp = stderr;
    if (log_file){
        fp = fopen(log_file, "a+");
    }
    if (fp==NULL){
        fp = stderr;
    }

    va_start(args, format);
    vfprintf(fp, format, args);
    fflush(fp);

    if (fp!=stderr){
        fclose(fp);
    }
    va_end(args);
}

static int addr_to_line(char *offset, char *fpath)
{
    if (fork()==0){
        dup2(pipe_fd[1], 1);
        close(pipe_fd[1]);
        if(execlp("addr2line", "addr2line", offset, "-e", fpath, NULL)<0){
            fprintf(stderr, "execl addr2line failed\n");
            exit(1);
        }
        exit(0);
    }
    char buf[1024] = {0};
    char *str = buf;
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 500000,
    };
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(pipe_fd[0], &readfds);
    if (select(pipe_fd[0] + 1, &readfds, NULL,  NULL, &tv)>0 && FD_ISSET(pipe_fd[0], &readfds)){
        do{
            read(pipe_fd[0], str, 1);
        }while(*str++!='\n');
        FD_ZERO(&readfds);
        FD_SET(pipe_fd[0], &readfds);
    }
    if (str!=buf){
        print_trace(buf);
    }
    return 0;
}

static int x86_trace(void **bt, int size)
{
    char **stacktrace = backtrace_symbols(bt, size);
    int i;
    int count = 0;
    for (i=0; i<size; i++){
        /* trace format is: binfile(symbol+offset) [addr] */
        char *trace = stacktrace[i];
        if (trace==NULL){
            continue;
        }
        char buf[1024] = {0};
        char offset[32] = {0};
        char *fpath = NULL;
        char *addr = NULL;
        char *symbol = NULL;
        char *token = NULL;
        char *fname = NULL;
        print_trace("%d: %s\t\t", count++, trace);
        /* get bin file name from trace */
        fpath = buf;
        strcpy(buf, trace);
        if ( (token = strchr(buf, '(')) == NULL){
            continue;
        }
        *token++ = '\0';

        /* get symbol form trace */
        symbol = token;
        if ( (token = strchr(token, ')')) == NULL){
            continue;
        }
        *token++ = '\0';
        if ( (token = strchr(token, '[')) == NULL){
            continue;
        }

        /* get addr from trace */
        addr = ++token;
        if ( (token = strchr(token, ']')) == NULL){
            continue;
        }
        *token = '\0';

        /* get file name from file path */
        if ( (fname = strrchr(fpath, '/')) ){
            fname++;
        }else{
            fname = fpath;
        }

		unsigned int symbol_offset = 0;
        /* check if bin file is .so */
        uint8_t **pc = NULL;
        Dl_info dl_info;
        if (strstr(fname, ".so") && *symbol!='\0'){
            pc = (uint8_t **)bt[i];
            /* if 'symbol' is not symbol name (just an address), or can't find it address from .so */
            if ( (dladdr((uint8_t *)pc, &dl_info)==0) || *symbol=='+'){
                /* if 'symbol' is address, we must use it as offset, or we can use 'addr' as offset */
                if (*symbol=='+'){
                    addr = symbol + 1;
                }
				strcpy(offset, addr);
            }else{
                /* if 'symbol' is symbol name, we must get it address of .so file.
                 * the dli_saddr is the absolute address in memory, the dli_fbase is the absolute address of .so.
                 * so dli_saddr - dli_fbase is the symbol's relatively address in .so file */
                unsigned int symbol_offset = (unsigned int)((unsigned long)dl_info.dli_saddr - (unsigned long)dl_info.dli_fbase);
				char *func_offset = strchr(symbol, '+');
				int f_offset = 0;
				if (func_offset){
					func_offset++;
					sscanf(func_offset, "%x", &f_offset);
				}
				symbol_offset += (f_offset - 1);
        		sprintf(offset, "0x%x", symbol_offset);
            }
        }else{
			sscanf(addr, "%x", &symbol_offset);
            if (symbol && *symbol!='+'){
                symbol_offset--;
            }
			sprintf(offset, "0x%x", symbol_offset);
        }

        addr_to_line(offset, fpath);
    }
    free(stacktrace);

    return 0;
}

static void on_sigsegv(int signum, siginfo_t *info, void *ptr)
{
    void *bt[100];
    int size = backtrace(bt, sizeof(bt)/sizeof(bt[0]));

    /* prevent log in a mess in case of multi threads*/
    pthread_mutex_lock(&trace_lock);

    time_t logtime = time(NULL);
    struct tm *local_time = localtime(&logtime);
    char *datetime = asctime(local_time);
    print_trace("\n------------------thread: 0x%x-----------------------\n", pthread_self());
    print_trace("get signal %s at time: %s", translate_signal(signum), datetime);
    print_trace("backtrace:\n");

    x86_trace( bt + 2, size - 2);

	pthread_mutex_unlock(&trace_lock);

    /* if another thread hold the lock, we can't exit, must wait it print it's log */
	sleep(1);
	if(pthread_mutex_trylock(&trace_lock) == 0)
	{
		pthread_mutex_unlock(&trace_lock);
    	fprintf(stderr,"get signal %s at time: %s", translate_signal(signum), datetime);
		if (access(log_file, F_OK)==0){
	    	fprintf(stderr,"please see log file %s\n", log_file);
		}
		exit(1);
	}
}

int trace_init(const char *logdir)
{
    int isdir = 1;
    if (access(logdir, F_OK)!=0){
        isdir = 0;
    }else{
        struct stat st;
        stat(logdir, &st);
        isdir = S_ISDIR(st.st_mode);
    }
    /* check log dir and make trace file name */
    if (!isdir){
        fprintf(stderr, "%s is not exist, use /tmp\n", logdir);
        logdir = "/tmp";
    }

    char *prog_name =NULL;
    char log_dir[256] = {0};
    int len = strlen(logdir);
    if (logdir[len-1]=='/'){
        len--;
    }
    strncpy(log_dir, logdir, len);
    prog_name = getpname();
    sprintf(log_file, "%s/%s.%d.trace",log_dir, prog_name, getpid());

    if (pipe(pipe_fd)!=0){
        return -1;
    }
	pthread_mutex_init(&trace_lock, NULL);

    /* create new stack for signal */
    stack_t ss;
    int s_size = 1024*128;
    ss.ss_sp = malloc(s_size);
    if (ss.ss_sp==NULL){
        fprintf(stderr, "failed to malloc\n");
        return -1;
    }
    ss.ss_size = s_size;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, NULL) == -1){
        fprintf(stderr, "failed to set signal stack\n");
        return -1;
    }

    /* install signal action */
    struct sigaction myAction;
    sigemptyset(&myAction.sa_mask);
    myAction.sa_sigaction = on_sigsegv;
    myAction.sa_flags = SA_SIGINFO|SA_ONSTACK;
    sigaction(SIGSEGV, &myAction, NULL);
    sigaction(SIGBUS, &myAction, NULL);
    sigaction(SIGABRT, &myAction, NULL);
    sigaction(SIGFPE, &myAction, NULL);
    sigaction(SIGILL, &myAction, NULL);
#if 0
    sigaction(SIGUSR1, &myAction, NULL);
    sigaction(SIGSYS, &myAction, NULL);
#endif
    return 0;
}
