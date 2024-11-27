#include "mysh.h"

/**
 * Executes parsed commands.
 * This file handles both built-in commands and external program executions.
 */

// Executes a single command (either built-in or external)
void run_command(command_t *cmd) {
    if (is_builtin_command(cmd->program)) {
        execute_builtin_command(cmd);
    } else {
        execute_external_program(cmd);
    }
}

// Handles the execution of built-in commands
void execute_builtin_command(command_t *cmd) {
    if (strcmp(cmd->program, "cd") == 0) {
        change_directory(cmd->arguments);
    } else if (strcmp(cmd->program, "pwd") == 0) {
        print_working_directory();
    } else if (strcmp(cmd->program, "which") == 0) {
        find_program_location(cmd->arguments);
    } else if (strcmp(cmd->program, "exit") == 0) {
        terminate_shell(cmd->arguments);
    }
}

// Executes an external program
void execute_external_program(command_t *cmd) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        setup_redirections(cmd);
        execv(cmd->program, cmd->arguments);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else if (child_pid > 0) {
        int exit_status;
        waitpid(child_pid, &exit_status, 0);
        handle_exit_status(exit_status);
    } else {
        perror("Forking failed");
    }
}

// Configures file redirections using dup2
void setup_redirections(command_t *cmd) {
    if (cmd->input_file) {
        int in_fd = open(cmd->input_file, O_RDONLY);
        if (in_fd < 0) {
            perror("Failed to open input file");
            exit(EXIT_FAILURE);
        }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    if (cmd->output_file) {
        int out_fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (out_fd < 0) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }
}

// Configures pipelines between two commands
void setup_pipeline(command_t *cmd1, command_t *cmd2) {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        return;
    }

    pid_t first_child = fork();
    if (first_child == 0) {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execv(cmd1->program, cmd1->arguments);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    }

    pid_t second_child = fork();
    if (second_child == 0) {
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execv(cmd2->program, cmd2->arguments);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(first_child, NULL, 0);
    waitpid(second_child, NULL, 0);
}
