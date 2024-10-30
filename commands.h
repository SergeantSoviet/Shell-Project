// Lukas Sarkauskas, 22415904 - Operating Systems Project 1
// ca216/myshell
// I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy

#ifndef COMMANDS_H
#define COMMANDS_H

int cd(char *args[]);
int clr(char *args[]);
int dir(char *args[]);
int my_environ(char *args[]);
void echo(char *args[]);
int help(char *args[]);
int pause_shell(char *args[]);
int quit(char *args[]);
int execute_program(char * args[]);

#endif 