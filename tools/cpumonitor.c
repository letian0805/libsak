#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static int pipe_fds[2];

static inline int epool_fd_nonblock(int fd)
{
    int flags = 0;
    int ret = -1;

    flags = fcntl (fd, F_GETFL);
    if (flags != -1){
        ret = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
    }
    return ret;
}

int parser

int main(int argc, char *argv[])
{
    pipe(pipe_fds);
    pid_t pid = fork();
    if (pid == 0){
        dup2(pipe_fds[1], 1);
        close(pipe_fds[1]);
        close(pipe_fds[0]);
        char *args[] = {"top", "-d", "1", NULL};
        execvp("top", args);
    }else{
        close(pipe_fds[1]);
        dup2(pipe_fds[0], 0);
        close(pipe_fds[0]);
        char buf[1024] = {0};
        while(fgets(buf, 1023, stdin)){
                printf("%s", buf);
                memset(buf, 0, 1024);
        }
    }

    return 0;
}
