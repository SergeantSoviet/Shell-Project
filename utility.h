// Lukas Sarkauskas, 22415904 - Operating Systems Project 1
// ca216/myshell
// I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy

#ifndef UTILITY_H
#define UTILITY_H

char **tokenize_input(char *input, int *in, int *out, int *append);
char execute(char **args, int in, int out, int append);
int backgroundexecute(char **args, int in, int out, int append);
int check_ampersand(char *args[]);
char *read_input(void);


#endif // UTILITY_H