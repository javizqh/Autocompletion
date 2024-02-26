#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>

#define MAX_MATCH_LENGTH 64

typedef struct _matches {
    char str[MAX_MATCH_LENGTH];
    struct _matches * next_match;
} matches;

int delete_matches(matches * first_match);

matches * autocomp(const char * line);

void print_matches(matches * first_match);

char * get_last_word(const char * line);