#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CMD_LENGTH 100
#define MAX_ARGS 10
#define HISTORY_SIZE 10

int history_count = 0;
char *histories[HISTORY_SIZE];

// 1. Task: cd <directory> command
void ex_cd(char *directory) {
    if (directory == NULL) {
        directory = getenv("HOME"); // Go to the home directory if no directory is specified
    }

    // Change to the specified directory (both absolute and relative paths)
    if (chdir(directory) == -1) {
        perror("chdir failed");  
    }
}

// 2. Task: pwd command (print current working directory)
void ex_pwd() {
    char cwd[MAX_CMD_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Get the current directory
        printf("%s\n", cwd);  // Print the current directory
    } else {
        perror("getcwd failed");  
    }
}

// Add new command to history
void add_to_histories(char *cmd) {
    if (history_count < HISTORY_SIZE) {
        histories[history_count++] = strdup(cmd);  // Add the command to history
    } else {
        free(histories[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            histories[i - 1] = histories[i];  // Shift older commands
        }
        histories[HISTORY_SIZE - 1] = strdup(cmd);  // Add new command at the end
    }
}

// Print command history
void ex_history() {
    for (int i = 0; i < history_count; i++) {
        printf("[%d] %s\n", i + 1, histories[i]);  // Print each command in history
    }
}

// Exit the shell
void ex_exit() {
    exit(0);
}

// Execute other commands (non-builtin)
void ex_other_commands(char *args[]) {
    pid_t pid = fork();  // Create a new process (child)
    if (pid == 0) {
        execvp(args[0], args);  // Execute the related command
        perror("execvp failed");  
        exit(1);
    } else {
        wait(NULL);  // Wait for the child process to finish
    }
}

// Handle piping between two commands
void ex_pipe_command(char *cmd1[], char *cmd2[]) {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");  
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child process 1: Execute the first command
        close(pipe_fd[0]);  // Close the read end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipe_fd[1]);  // Close the write end of the pipe

        execvp(cmd1[0], cmd1);  // Execute the first command
        perror("execvp failed");  
        exit(1);
    } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Child process 2: Execute the second command
            close(pipe_fd[1]);  // Close the write end of the pipe
            dup2(pipe_fd[0], STDIN_FILENO);  // Redirect stdin from the pipe
            close(pipe_fd[0]);  // Close the read end of the pipe

            execvp(cmd2[0], cmd2);  // Execute the second command
            perror("execvp failed");  
            exit(1);
        }

        close(pipe_fd[0]);  // Close both ends of the pipe in the parent process
        close(pipe_fd[1]);  

        waitpid(pid1, NULL, 0);  // Wait for the first child process
        waitpid(pid2, NULL, 0);  // Wait for the second child process
    }
}

// Handle background processes
void ex_background_processes(char *args[]) {
    pid_t pid = fork();  // Create a new process (child)
    if (pid == 0) {
        execvp(args[0], args);  // Execute the background command
        perror("execvp failed");  
        exit(1);
    } else {
        printf("Background process ID: %d\n", pid);  // Print the background process ID
    }
}

// Check if a command is a built-in shell command (cd, pwd, history, exit)
int is_builtin(char *args[]) {
    if (strcmp(args[0], "cd") == 0) return 1;
    if (strcmp(args[0], "pwd") == 0) return 1;
    if (strcmp(args[0], "history") == 0) return 1;
    if (strcmp(args[0], "exit") == 0) return 1;
    return 0;
}

// Execute built-in commands
void execute_builtin(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        ex_cd(args[1]);
    } else if (strcmp(args[0], "pwd") == 0) {
        ex_pwd();
    } else if (strcmp(args[0], "history") == 0) {
        ex_history();
    } else if (strcmp(args[0], "exit") == 0) {
        ex_exit();
    }
}

// Parse a command into arguments
void parse_command(char *cmd, char *args[]) {
    cmd[strcspn(cmd, "\n")] = 0;  // Remove the newline character

    char *token = strtok(cmd, " ");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {  // Avoid buffer overflow
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // Null-terminate the argument array
}

// Execute logical AND (&&) commands
void ex_and_command(char *cmd1[], char *cmd2[]) {
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Execute the first command
        execvp(cmd1[0], cmd1);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid1, &status, 0);  // Wait for the first command to finish
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            // If the first command succeeded, execute the second command
            pid_t pid2 = fork();
            if (pid2 == 0) {
                execvp(cmd2[0], cmd2);
                perror("execvp failed");
                exit(1);
            } else {
                wait(NULL);
            }
        } else {
            // If the first command failed, do not run the second command
            printf("First command failed, skipping second command.\n");
        }
    }
}


int main() {
    char cmd[MAX_CMD_LENGTH];
    char *args[MAX_ARGS];

    while (1) {
        printf("shell322>");

        if (fgets(cmd, MAX_CMD_LENGTH, stdin) == NULL) {
            continue;
        }

        cmd[strcspn(cmd, "\n")] = 0;  // Remove the newline character
        add_to_histories(cmd);

        char tempCmd[MAX_CMD_LENGTH];
        strcpy(tempCmd, cmd);  // Make a copy of the command for later processing

        parse_command(cmd, args);

        // Detect the size of args (Otherwise I don't know how can I detect it)
        int argsSize = 0;
        for (int i = 0; i < MAX_ARGS && args[i] != NULL; i++) {
            argsSize++;
        }

        // Execute built-in commands
        if (is_builtin(args)) {
            execute_builtin(args);
        } 

        // Handle non-built-in commands and piping
        else if (args[0] != NULL && strcmp(args[0], "exit") != 0) {

            // If pipe character '|' is detected in the command
            if (strchr(tempCmd, '|')) {
                char *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];

                char *pipe_pos = strchr(tempCmd, '|');
                *pipe_pos = '\0';  // Separate the two commands

                parse_command(tempCmd, cmd1);  // First command
                parse_command(pipe_pos + 1, cmd2);  // Second command

                // Execute pipe command
                ex_pipe_command(cmd1, cmd2);
            }
            else if (strstr(tempCmd, "&&")) { // Look at the tempCmd for search && string
                char *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];

                char *and_pos = strstr(tempCmd, "&&"); // Find position of the && string
                *and_pos = '\0';  // Separate the two commands

                parse_command(tempCmd, cmd1);  // First command
                parse_command(and_pos + 2, cmd2);  // Second command

                // Execute pipe command
                ex_and_command(cmd1, cmd2);
            }

            // If background process '&' is detected
            else if (args[argsSize - 1] != NULL && strcmp(args[argsSize - 1], "&") == 0) {
                args[argsSize - 1] = NULL;  // Remove '&' from the command
                ex_background_processes(args);  // Execute the background process
            } 
            else {
                ex_other_commands(args);  // Execute normal command
            }
        }
    }
    return 0;
}