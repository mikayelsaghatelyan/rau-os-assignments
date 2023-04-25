#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

bool debug = false;

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Invalid number of arguments (minimum is 3).\n");
        return 1;
    }
    int pipe_num = argc - 2;
    int proc_num = argc - 1;
    int pipe_fd[pipe_num][2];   //pipes
    pid_t pids[proc_num];       //processes

    // pipe_fd[0][0]            - pipe between processes (0) -> (1) read
    // pipe_fd[0][1]            - pipe between processes (0) -> (1) write
    // pipe_fd[1][0]            - pipe between processes (1) -> (2) read
    // pipe_fd[1][1]            - pipe between processes (1) -> (2) write
    // ...
    // ...
    // pipe_fd[pipe_num - 1][0] - pipe between processes (proc_num - 2) -> (proc_num - 1) read
    // pipe_fd[pipe_num - 1][1] - pipe between processes (proc_num - 2) -> (proc_num - 1) read

    // creating pipes
    for (int i = 0; i < pipe_num; ++i) {
        if (debug) std::cout << "creating pipe " << i + 1 << " to " << i + 2 << "\n";
        if (pipe(pipe_fd[i]) < 0) {
            fprintf(stderr, "Failed to create pipe (%d) -> (%d).\n", i + 1, i + 2);
            return 1;
        }
    }
    // closing pipes
    for (int i = 0; i < pipe_num; ++i) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }
    // forking processes
    for (int i = 0; i < proc_num; ++i) {
        if (debug) std::cout << "forking process " << i + 1 << "\n";
        pids[i] = fork();
        if (pids[i] < 0) {
            fprintf(stderr, "Failed to fork child process %d.\n", i + 1);
        }
        if (pids[i] == 0) {
            if (debug) std::cout << "for process " << i + 1 << ":\n";
            if (i == 0) {
                for (int j = 1; j < pipe_num; ++j) {
                    if (debug) std::cout << "closing pipe " << j + 1 << " to " << j + 2 << "\n";
                    close(pipe_fd[j][0]);
                    close(pipe_fd[j][1]);
                }
                close(pipe_fd[i][0]);
                dup2(pipe_fd[i][1], STDOUT_FILENO);
                close(pipe_fd[i][1]);
            } else if (i == pipe_num - 1) {
                for (int j = 0; j < pipe_num - 1; ++j) {
                    if (debug) std::cout << "closing pipe " << j + 1 << " to " << j + 2 << "\n";
                    close(pipe_fd[j][0]);
                    close(pipe_fd[j][1]);
                }
                close(pipe_fd[i][1]);
                dup2(pipe_fd[i][0], STDIN_FILENO);
                close(pipe_fd[i][0]);
            } else {
                for (int j = 0; j < pipe_num - 1; ++j) {
                    if (j != i && j != i + 1) {
                        if (debug) std::cout << "closing pipe " << j + 1 << " to " << j + 2 << "\n";
                        close(pipe_fd[j][0]);
                        close(pipe_fd[j][1]);
                    }
                }
                close(pipe_fd[i - 1][1]);
                dup2(pipe_fd[i - 1][0], STDIN_FILENO);
                close(pipe_fd[i - 1][0]);

                close(pipe_fd[i][0]);
                dup2(pipe_fd[i][1], STDOUT_FILENO);
                close(pipe_fd[i][1]);
            }
            if (execlp(argv[i + 1], argv[i + 1], NULL) < 0) {
                fprintf(stderr, "Failed to execute process %d.\n", i + 1);
                return 1;
            }
        }
    }
    // closing pipes again
    for (int i = 0; i < pipe_num; ++i) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }
    // waiting for child processes
    for (int i = 0; i < proc_num; ++i) {
        waitpid(pids[i], NULL, 0);
    }
    return 0;
}