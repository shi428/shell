#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

int main() {
    int fds[2];
    pipe(fds);

    char *cmd1[3] = {"cat", "/usr/share/dict/words", NULL};
    char *cmd2[4] = {"tee", "-a", "file1", NULL};
    pid_t child[2];
    int status;
    child[0] = fork();
    if (child[0] == 0) {
        dup2(fds[1], 1);
        close(fds[0]);
        execvp(cmd1[0], cmd1);
    }
    //else {
    //    waitpid(child, &status, 0);
    //}
    close(fds[1]);
    child[1] = fork();
    printf("done\n");
    if (child[1] == 0) {
        dup2(fds[0], 0);
        close(fds[1]);
        int null = open("/dev/null", O_CREAT | O_WRONLY | O_TRUNC);
        dup2(null, 1);
        close(null);
        execvp(cmd2[0], cmd2);
    }
    //else {
    //    waitpid(child, &status, 0);
    //}
    close(fds[0]);
    for (int i = 0; i < 2; i++) {
        wait(NULL);
        //waitpid(child[i], &status, 0);
    }
    printf("done\n");
}
