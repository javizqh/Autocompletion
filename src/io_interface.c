#include <io_interface.h>

typedef struct _line {
    char prompt[MAX_PROMPT_LENGTH];
    char prev_command[MAX_COMMAND_LENGTH];
} line;

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

void add_char(char * base_str, char c, int pos) {
        int len;
	char *buf;

        if (strlen(base_str) == 0) {
                *base_str = c;
                return;
        }

        buf = malloc(MAX_COMMAND_LENGTH);
	if (buf == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(buf, 0, MAX_COMMAND_LENGTH);

        strncpy(buf, base_str, pos); // copy at most first pos characters
        len = strlen(buf);
        strcpy(buf+len, &c); // copy all of insert[] at the end
        len ++;  // increase the length by length of insert[]
        strcpy(buf+len, base_str+pos); // copy the rest

        strcpy(base_str, buf);   // copy it back to subject

        free(buf);
        return;
}

void del_char(char * base_str, int pos) {
	char *buf;

        if (strlen(base_str) == 0) {
                return;
        }

        buf = malloc(MAX_COMMAND_LENGTH);
	if (buf == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(buf, 0, MAX_COMMAND_LENGTH);

        strncpy(buf, base_str, pos - 1); // copy at most first pos characters
        strcpy(buf+strlen(buf), base_str+pos); // copy the rest
        strcpy(base_str, buf);   // copy it back to subject

        free(buf);
        return;
}

void show_line(char *prompt) {
        printf("%s ", prompt);
        fflush(stdout);
};

void refresh_line(char *prompt, char *buffer, int cursor_pos) {
        gotoy(1);
        printf("%*c", (int) (strlen(prompt) + strlen(buffer) + 2), ' ');
        gotoy(1);
        printf("%s ", prompt);
        printf("%s", buffer);
        fflush(stdout);
        gotoy(cursor_pos);
}