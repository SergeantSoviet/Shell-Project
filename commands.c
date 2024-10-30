// Lukas Sarkauskas, 22415904 - Operating Systems Project 1
// ca216/myshell
// I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy

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

#define MAX_BUFFER 1024
#define SEPARATORS " \t\n"

// Function to change directory
int cd(char *args[]) {
    if (args[1] == NULL) {
        // If no argument is provided, print current directory
        char cwd[MAX_BUFFER];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current directory: %s\n", cwd);
            return 0;
        }
        else {
            perror("getcwd");
            return -1;
        }
    }
    else {
        // Change directory to the provided path
        if (chdir(args[1]) != 0) {
            perror("cd");
            return -1;
        }
        else {
            // Update environment variable PWD
            char cwd[MAX_BUFFER];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                setenv("PWD", cwd, 1);
                return 0;
            }
            else {
                perror("getcwd");
                return -1;
            }
        }
    }
}

// Function to clear the screen
int clr(char *args[]) {
    printf("\033[H\033[J"); // ANSI escape sequence for clearing screen;
    return 0;
}

// Function to list contents of current directory
int dir(char *args[]) {
    if (args[1] == NULL) {
        // If no argument is provided, list current directory
        int check = system("ls -al");
        if (check == -1) {
            perror("dir");
            return -1;
        }
    }
    else {
        // List the provided directory
        char command[256];
        sprintf(command, "ls -al %s", args[1]); // args[1] should be the directory name
        int check = system(command);
        if (check == -1) {
            perror("dir");
            return -1;
        }
    }
    return 0;
}

// Function to print environment variables
int my_environ(char *args[]) {
    extern char **environ;
    for (char **env = environ; *env; ++env)
        printf("%s\n", *env);
    return 0;
}

// Function to echo arguments
void echo(char *args[]) {
    FILE *file = stdout;
    for (int i = 1; args[i]; i++) {
        if (strcmp(args[i], ">") == 0) {
            // If ">" found, redirect output to file
            file = fopen(args[i + 1], "w");
            if (file == NULL) {
                perror("echo");
                return;
            }
            args[i] = NULL; 
            break;
        }
    }

    for (int i = 1; args[i]; i++)
        fprintf(file, "%s ", args[i]);
    fprintf(file, "\n");

    if (file != stdout) 
        fclose(file);
}

// Function to print user manual
int help(char *args[]) {
    
    if (system("more ../manual/readme.md") != 0) {
        printf("User manual not found or more command failed.\n");
        return 1;
    }
    return 0;
}
// Function to pause shell until Enter is pressed
int pause_shell(char *args[]) {
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    return 0;
}

// Function to exit the shell
int quit(char *args[]) {
    exit(0);
}

// Function to execute an external program
int execute_program(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        setenv("parent", "/myshell", 1); // Set the environment variable
        if (execv(args[0], args) == -1) {
            perror("execv");
            return -1;
        }
    } else if (pid < 0) {
        // Error forking
        perror("fork");
        return -1;
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, WUNTRACED);
    }
    return 0;
}
