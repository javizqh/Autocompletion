#include <autocomp.h>

#define MAX_PATH_SIZE 1024*10
#define MAX_ENV_SIZE 1024

matches * add_match(matches * prev_match, const char value[MAX_MATCH_LENGTH]) {
    matches * match = (matches *) malloc(sizeof(matches));

	if (match == NULL) err(EXIT_FAILURE, "malloc failed");

	memset(match, 0, sizeof(matches));

    strcpy(match->str, value);
    match->next_match = NULL;

    if (prev_match != NULL) prev_match->next_match = match;
    return match;
}

int delete_matches(matches * first_match) {
    matches * free_match = first_match;
    
    for (matches * next_match = free_match; next_match;) {
        next_match = free_match->next_match;
        free(free_match);
        free_match = next_match;
    }
    
    return 1;
}

char * get_last_word(const char * line) {
    int len = strlen(line);
    int start_word = len;

    if (len == 0) return NULL;

    for (start_word; start_word > 0; start_word--) {
        if (line[start_word - 1] == ' ') break;
    }

    start_word = (start_word > 0) ? start_word +1 : 0; // Skip empty space

    char * last_word = malloc(len - start_word + 1);
    if (last_word == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(last_word, 0, len - start_word + 1);

    char *ptr = last_word;
    
    for (start_word; start_word < len; start_word++) {
        *ptr++ = *(line + start_word);
    }

    return last_word;
}

matches * search_in_dir(const char * dirname, matches * last_match, const char * word, int search_dir) {
    DIR *dir;
    matches * curr_match = last_match;
    char * real_dirname = malloc(128);
	if (real_dirname == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(real_dirname, 0, 128);

	if ((dir = opendir(dirname)) == NULL) {
        free(real_dirname);
		return last_match;
	}
	// Iterate normal
	struct dirent *file_info;

	while ((file_info = readdir(dir)) != NULL) {
		if (file_info->d_type == DT_DIR && search_dir == 1) {
            if (strncmp(file_info->d_name, word, strlen(word)) == 0) {
                memset(real_dirname, 0, 128);
                strcpy(real_dirname, file_info->d_name);
                strcat(real_dirname, "/");
                if (strlen(last_match->str) == 0) strcpy(last_match->str, real_dirname);
                else curr_match = add_match(curr_match, real_dirname);
            }
		} else if (file_info->d_type == DT_REG) {
			if (strncmp(file_info->d_name, word, strlen(word)) == 0) {
                if (strlen(last_match->str) == 0) strcpy(last_match->str, file_info->d_name);
                else curr_match = add_match(curr_match, file_info->d_name);
            }

		}
	}
    free(real_dirname);
	closedir(dir);
    return curr_match;
}

matches * autocomp(const char * line) {
    // First get the last word
    char * last_word = get_last_word(line);

    if (last_word == NULL) return NULL;

    matches * match = (matches *) malloc(sizeof(matches));
	if (match == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(match, 0, sizeof(matches));

    match->next_match = NULL;
    matches * curr_match = match;
/////////////////////////////////////////////////////////////
	char * cwd, *orig_cwd, *cwd_ptr;
	char *path, *orig_path, *path_ptr;
	char *token;

	// SEARCH IN ABSOLUTE PATH
	if (*last_word == '/') {
		curr_match = search_in_dir(last_word, curr_match, last_word, 1);
        goto end;
	}

	// CHECK IN PWD
	cwd = malloc(MAX_ENV_SIZE);
	if (cwd == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(cwd, 0, MAX_ENV_SIZE);
	// Copy the path
    orig_cwd = getenv("PWD");
	if (orig_cwd == NULL) {
		free(cwd);
		goto end;
	}
    strcpy(cwd, orig_cwd);
	cwd_ptr = cwd;

	strcat(cwd_ptr, "/");
    curr_match = search_in_dir(cwd, curr_match, last_word, 1);
	free(cwd);

	// SEARCH IN PATH
	// First get the path from env PATH
	path = malloc(MAX_PATH_SIZE);
	if (path == NULL) err(EXIT_FAILURE, "malloc failed");
	memset(path, 0, MAX_PATH_SIZE);
	// Copy the path
	orig_path = getenv("PATH");
	if (orig_path == NULL || strlen(orig_path) > MAX_PATH_SIZE - 1) {
		free(path);
		goto end;
	}
	strcpy(path, orig_path);
	path_ptr = path;

	// Then separate the path by : using strtok

	while ((token = strtok_r(path_ptr, ":", &path_ptr))) {
        curr_match = search_in_dir(token, curr_match, last_word, 0);
	}

	free(path);

end:
    free(last_word);
    if (strlen(match->str) == 0) {
        free(match);
        return NULL;
    }
    return match;
}

void print_matches(matches * first_match) {
    int max_per_line = 3;
    int curr_per_line = 0;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf("\n");

    for (matches *ptr = first_match; ptr != NULL; ptr = ptr->next_match) {
        printf("%-*s",w.ws_col/4, ptr->str);
        if (++curr_per_line >= max_per_line) {
            curr_per_line = 0;
            printf("\n");
        }
    }

    if (curr_per_line != 0) printf("\n");
    
    return;
}