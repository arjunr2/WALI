#include "common.h"
#define NUM_ITS 15
#define OFF 7
#define MS 1000000

int main()
{
    int pipefd[2];
    pid_t cpid;
    char buf[100];
    char write_str[] = "test pipe: DEADBEEF\n";
    int* write_pt = (int*) &write_str[strlen(write_str)-OFF];

    struct timespec sleep_ts = {
      .tv_sec = 0,
      .tv_nsec = 100 * MS
    };
    struct timespec rem_ts = {0};

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = syscall(SYS_fork);
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Child reads from pipe */
        PRINT_INT("Child PID", getpid());
        close(pipefd[1]);          /* Close unused write end */

        while (read(pipefd[0], &buf, strlen(write_str)) > 0)
            write(STDOUT_FILENO, &buf, strlen(write_str));

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);

    } else {            /* Parent writes argv[1] to pipe */
        PRINT_INT("Parent PID", getpid());
        close(pipefd[0]);          /* Close unused read end */
        for (int i = 0; i < NUM_ITS; i++) {
          write(pipefd[1], write_str, strlen(write_str));
          *write_pt += 1;
          nanosleep(&sleep_ts, &rem_ts); /* Wait 1 sec */
        }
        close(pipefd[1]);          /* Reader will see EOF */
    }
    return 0;
}

