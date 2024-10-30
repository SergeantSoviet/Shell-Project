// Lukas Sarkauskas, 22415904 - Operating Systems Project 1
// ca216/myshell
// I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy.


#include "commands.h"
#include <stdio.h>   
#include <unistd.h>  
#include <stdlib.h>  
#include <dirent.h>  
#include <string.h>  
#include <sys/stat.h>  
#include <sys/types.h> 
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "utility.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_BUFFER 1024
#define SEPARATORS " \t\r\n\a"


// Function to tokenize the input line
char** tokenize_input(char* line, int* in, int* out, int* append) {
    int bufsize = MAX_BUFFER;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    *in = -1;
    *out = -1;
    *append = 0;

    if (!tokens) {
        fprintf(stderr, "myShell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SEPARATORS);
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, SEPARATORS);
            *in = open(token, O_RDONLY);
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, SEPARATORS);
            *out = open(token, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        }
        else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, SEPARATORS);
            *out = open(token, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            *append = 1;
        } else {
            if (token[0] == '"' && token[strlen(token) - 1] == '"') {
                token[strlen(token) - 1] = '\0';
                token++;
            }
            tokens[position] = token;
            position++;
        }

        if (position >= bufsize) {
            bufsize += MAX_BUFFER;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myShell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SEPARATORS);
    }
    tokens[position] = NULL;
    return tokens;
}

// Function to check if the last argument is "&"
int check_ampersand(char *args[]) {
    int index = 0;
    while (args[index] != NULL) {
        ++index;
    }
    // Check if args is empty
    if (index == 0) {
        return 0;
    }
    if (!strcmp(args[index - 1], "&")) {
        return 1;
    }
    else {
        return 0;
    }
}

// Function to execute commands
char execute(char **args, int in, int out, int append) {
    int background = check_ampersand(args);
    if (background) {
        int i = 0;
        while (args[i] != NULL){
            i++;
        }
        args[i - 1] = NULL;
    }

    if (!strcmp(args[0], "cd")) {
        cd(args);
    }
    else if (!strcmp(args[0], "clr")) {
        clr(args);
    }
    else if (!strcmp(args[0], "dir")) {
        dir(args);
    }
    else if (!strcmp(args[0], "environ")) {
        my_environ(args);
    }
    else if (!strcmp(args[0], "echo")) {
        echo(args);
    }
    else if (!strcmp(args[0], "help")) {
        help(args);
    }
    else if (!strcmp(args[0], "pause")) {
        pause_shell(args);
    }
    else if (!strcmp(args[0], "quit")) {
        quit(args);
    }
    else {
        if (background) {
            backgroundexecute(args, in, out, append);
        } else {
            pid_t pid = fork(); // creates a child process
            if (pid == 0) {
                if (in != -1) {
                    dup2(in, STDIN_FILENO);
                    close(in);
                }
                if (out != -1) {
                    dup2(out, STDOUT_FILENO);
                    close(out);
                }
                if (execvp(args[0], args) == -1) {
                    perror("execute");
                }
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                perror("execute");
            } else {
                wait(NULL);
            }
        }
    }
    
    return 0;
}

// Function to execute commands in background
int backgroundexecute(char **args, int in, int out, int append) {
    int background = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            args[i] = NULL; // Remove the '&' from the args
            background = 1;
            break;
        }
    }

    pid_t pid = fork(); // creates a child process
    if (pid == 0) {
        if (in != -1) {
            dup2(in, STDIN_FILENO);
            close(in);
        }
        if (out != -1) {
            dup2(out, STDOUT_FILENO);
            close(out);
        }
        if (execvp(args[0], args) == -1) {
            printf("\nError, command \"%s\" not found. Running command in system shell.\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }
    else if(pid == -1) {
        printf("Error, fork failed.\n");
    }
    else {
        if (!background) {
            int status;
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 0;
}

char *read_input(void) {
    char *line = NULL;
    size_t bufsize = 0; 
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s $ ", cwd);
    } else {
        perror("getcwd() error");
        return NULL;
    }

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  
        } else  {
            perror("read_input");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}
