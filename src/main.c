#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

#include <io_interface.h>
#include <autocomp.h>

#define MAX_FILENAME_SIZE 1024
#define PROMPT "\033[01;35mC project\033[00m:\033[01;34m~/Code/C/Autocompletion\033[00m $"

#define N_ARGS 0
#define N_OPT_ARGS 2

typedef struct args {
    char file [MAX_FILENAME_SIZE];
} * Args;

int has_to_exit = 0;

Args check_args(int argc, char *const *argv);
void sig_handler(int sig);

void usage() {
    fprintf(stderr, "usage: ./main [--file FILENAME]\n");

    exit(EXIT_FAILURE);
}

int main(int argc, char *const *argv)
{
    int status;

    Args arguments = check_args(argc, argv);
    
    signal(SIGPIPE, sig_handler);

    // ---------- Read command line
	// ------ Buffer
	char *buf = malloc(MAX_COMMAND_LENGTH);
	if (buf == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(buf, 0, MAX_COMMAND_LENGTH);
	// ------------

    char character;
    int len = 0, max_len = 0;
    int n_tabs = 0;
    int prompt_len = strlen("C project:~/Code/C/Autocompletion $") + 1;

    show_line(PROMPT);

	while (1) {	/* break with ^D or ^Z */
        // TODO: hashify
        character = getch();
        printf("%d", character);
        switch (character)
        {
        case '\n':
            printf("\nRead %s", buf);
            printf("\n");
            show_line(PROMPT);
            memset(buf, 0, MAX_COMMAND_LENGTH);
            len = 0, max_len = 0, n_tabs = 0;
            break;
        case '\t':
            if (n_tabs++) {
                matches * complete = autocomp(buf);
                printf("\a"); // Alarm
                if (complete == NULL) break; // If no match exit
                if (complete->next_match == NULL) { // Substitute word for match
                    char * last_word = get_last_word(buf);
                    len = (len - strlen(last_word) > 0) ? len - strlen(last_word) : 0;
                    free(last_word);
                    strcpy(&buf[len], complete->str);
                    gotoy(1);
                } else {
                    print_matches(complete);
                }
                delete_matches(complete);
                show_line(PROMPT);
                printf("%s", buf);
            }
            break;
        case 127:
            del_char(buf, len);
            len = (len > 0) ? len - 1 : 0;
            refresh_line(PROMPT, buf, len + 1 + prompt_len);
            break;
        case '^':
            printf("\n");
            show_line(PROMPT);
            memset(buf, 0, MAX_COMMAND_LENGTH);
            len = 0, max_len = 0, n_tabs = 0;
            break;
        case 27:
            getch(); // Remove the [
            switch(getch()) {
                case 'A':
                    // printf("Up\n");
                    break;
                case 'B':
                    // printf("Down\n");
                    break;
                case 'C':
                    // printf("Right\n");
                    len = (len < max_len) ? len + 1 : max_len;
                    gotoy(len + 1 + prompt_len);
                    break;
                case 'D':
                    printf("\b");
                    len = (len > 0) ? len - 1 : 0;
                    break;
            }
            break;
        default:
            // buf[len++] = character;
            add_char(buf, character, len++);
            max_len = (len > max_len) ? len : max_len;
            refresh_line(PROMPT, buf, len + 1 + prompt_len);
            break;
        }
        fflush(stdout);
        
		if (has_to_exit) {
			break;
		}
	}

	if (ferror(stdin)) {
		err(EXIT_FAILURE, "fgets failed");
	}

	free(buf);
	return 0;
}

Args check_args(int argc, char *const *argv) {
    int c;
    int opt_index = 0;
    static struct option long_options[] = {
        {"file",        required_argument, 0,  0 },
        {0,             0,                 0,  0 }
    };
    Args arguments = malloc(sizeof(struct args));

    if (arguments == NULL) err(EXIT_FAILURE, "Failed to allocate arguments");
    memset(arguments, 0, sizeof(struct args));

    if (argc - 1 == N_ARGS) {
        free(arguments);
        return NULL;
    } else if (argc - 1 != N_OPT_ARGS) {
        free(arguments);
        usage();
    }

    while (1) {
        opt_index = 0;

        c = getopt_long(argc, argv, "", long_options, &opt_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            if (strcmp(long_options[opt_index].name, "file") == 0) {
                strcpy(arguments->file ,optarg);
            }
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        free(arguments);
        usage();
    }
    return arguments;
}

void sig_handler(int sig) {
	switch (sig) {
    case SIGPIPE:
        has_to_exit = 1;
        break;
    // case SIGINT:
    //     printf("^C\n");
    //     show_line(PROMPT);
    //     // FIX: clean buffer
    //     break;
	default:
		break;
	}
}
