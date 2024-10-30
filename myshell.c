// Lukas Sarkauskas, 22415904 - Operating Systems Project 1
// ca216/myshell
// I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utility.h"

int main(int argc, char *argv[]) {
    char cwd[1024];
    char *input;
    char **args;
    int ampersand;
    int in, out, append;

    char prompt[1024];
    getcwd(prompt, sizeof(prompt));
    strcat(prompt, "/myshell");
    setenv("SHELL", prompt, 1);


    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }
        char line[1024];
        while (fgets(line, sizeof(line), file)) {
            args = tokenize_input(line, &in, &out, &append);
            ampersand = check_ampersand(args);

            if (ampersand) {
                backgroundexecute(args, in, out, append);
            } else {
                execute(args, in, out, append);
            }

            free(args);
        }
        fclose(file);
    } else {
        while (1) {
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                perror("getcwd() error");
                return 1;
            }
            input = read_input();
            if (input == NULL || strlen(input) == 0) { // Check if input is empty
                free(input);
                continue;
            }
            args = tokenize_input(input, &in, &out, &append);
            if (args[0] == NULL) { // Check if args is empty
                free(args);
                continue;
            }
            ampersand = check_ampersand(args);

            if (ampersand) {
                backgroundexecute(args, in, out, append);
            } else {
                execute(args, in, out, append);
            }

            free(input);
            free(args);
        }
    }

    return 0;
}