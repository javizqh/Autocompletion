#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include<errno.h>

#define MAX_PROMPT_LENGTH 128
#define MAX_COMMAND_LENGTH 1024

#define gotoy(y) printf("\033[%dG", (y))

char getch();
void show_line(char *prompt);
void add_char(char * line, char c, int pos);
void del_char(char * base_str, int pos);
void refresh_line(char *prompt, char *buffer, int cursor_pos);