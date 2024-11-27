#include "executor.h"
#include "builtin.h"
#include "utility.h"

void execute_command(Command *cmd) {
    if (is_builtin(cmd->name)) {
        execute_builtin(cmd);
    } else {
        execute_external(cmd);
    }
}

void execute_builtin(Command *cmd) {
    if (strcmp(cmd->name, "cd") == 0) {
        cmd_cd(cmd->args);
    } else if (strcmp(cmd->name, "pwd") == 0) {
        cmd_pwd();
    } else if (strcmp(cmd->name, "which") == 0) {
        cmd_which(cmd->args);
    } else if (strcmp(cmd->name, "exit") == 0) {
        cmd_exit(cmd->args);
    }
}

void execute_external(Command *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        error_handler("Fork failed");
        return;
    }
    if (pid == 0) {  // Child process
        handle_redirection(cmd);
        execv(cmd->path, cmd->args);
        error_handler("Execution failed");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code != 0) {
                printf("Command failed with code %d\n", code);
            }
        } else if (WIFSIGNALED(status)) {
            printf("Terminated by signal: %d\n", WTERMSIG(status));
        }
    }
}

void handle_redirection(Command *cmd) {
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) error_handler("Failed to open input file");
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->output_file) {
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd < 0) error_handler("Failed to open output file");
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

void handle_pipe(Command *cmd1, Command *cmd2) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        error_handler("Pipe failed");
        return;
    }
    pid_t pid1 = fork();
    if (pid1 == 0) {  // First child
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        execv(cmd1->path, cmd1->args);
        exit(EXIT_FAILURE);
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {  // Second child
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        execv(cmd2->path, cmd2->args);
        exit(EXIT_FAILURE);
    }
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

