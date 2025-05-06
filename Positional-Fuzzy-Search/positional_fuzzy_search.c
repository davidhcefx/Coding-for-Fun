/**
 * Case insensitive, positional fuzzy search, sorted by scores.
 * Written by davidhcefx, 2025.5.6.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_INPUT 128

typedef struct {
	int score;
	char *str;
} match_t;


static int __match_compare(const void *a, const void *b)
{
	const match_t *ma = (const match_t *)a;
	const match_t *mb = (const match_t *)b;
	return (ma->score - mb->score);
}

/**
 * @brief Perform positional match
 * 	 For example, "ass" should match "apply the settings".
 * @param needle The key to search. Every char should appear in hay in order.
 * @param hay The string to be searched.
 * @return Number of consecutive matched characters, or 0 if no matched
 */
int positional_match(const char *hay, const char *needle)
{
	int consec = 0;
	const char *prev;
	if (*needle) {
		hay = strchr(hay, *needle);
		if (!hay) {
			return 0;
		}
		consec++;  // count for the initial match
		hay++;
		needle++;
	}
	while (*needle) {
		prev = hay;
		hay = strchr(hay, *needle);
		// printf("%s: %p\n", hay, hay);
		if (!hay) {
			return 0;
		}
		if (hay == prev) {
			consec++;
		}
		hay++;
		needle++;
	}
	return consec;
}

bool starts_with(char *str, char c)
{
	return str[0] == c;
}

char *s_tolower(char *str, char *buf, size_t n)
{
	size_t i;
	for (i = 0; i < n - 1 && *str; i++, str++) {
		buf[i] = tolower(*str);
	}
	buf[i] = '\0';
	return buf;
}

char **list_construct(FILE *fp, size_t *nb_list)
{
	size_t max_size = 64;  // initial size
	*nb_list = 0;
	char **list = malloc(sizeof(char *) * max_size);
	if (!list) {
		perror("malloc");
		return NULL;
	}
	// read each line
	char *lineptr = NULL;
	size_t linesize = 0;
	int len;
	while ((len = getline(&lineptr, &linesize, fp)) > 0) {
		if (lineptr[len - 1] == '\n') {
			lineptr[len - 1] = '\0';  // trim newline
		}
		list[*nb_list] = strdup(lineptr);
		if (!list[*nb_list]) {
			continue;
		}
		*nb_list += 1;
		if (*nb_list >= max_size) {
			max_size *= 2;
			char **new_list = realloc(list, sizeof(char *) * max_size);
			if (!new_list) {
				perror("realloc");
				free(list);
				return NULL;
			}
			list = new_list;
		}
	}
	free(lineptr);
	return list;
}

void list_destroy(char **list, size_t nb_list)
{
	size_t i;
	for (i = 0; i < nb_list; i++) {
		free(list[i]);
	}
	free(list);
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Syntax: %s <input_list.txt>\n", argv[0]);
		return 0;
	}
	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		perror("fopen");
		return -1;
	}
	size_t nb_list = 0;
	char **list = list_construct(fp, &nb_list);
	fclose(fp);
	if (!list) {
		return -1;
	}
	char input[MAX_INPUT] = {0};  // user input
	size_t in_idx = 0;
	match_t matches[nb_list];
	size_t mch_idx;
	char buf[MAX_INPUT];
	char *hay;
	int score;
	size_t i;
	while (in_idx < MAX_INPUT) {
		char c = tolower(getchar());
		if (c == '\n') {  // filter out newlines
			continue;
		}
		input[in_idx++] = c;
		// filter for candidates
		mch_idx = 0;
		for (i = 0; i < nb_list; i++) {
			hay = s_tolower(list[i], buf, sizeof(buf));
			score = positional_match(hay, input);
			if (score > 0) {
				matches[mch_idx].score = score + (starts_with(hay, input[0]) ? 1 : 0);
				matches[mch_idx].str = list[i];
				mch_idx++;
			}
		}
		// best matches are at the bottom
		qsort(matches, mch_idx, sizeof(match_t), __match_compare);
		for (i = 0; i < mch_idx; i++) {
			printf("  %u: %s\n", matches[i].score, matches[i].str);
		}
		printf("your input: %s\n", input);
	}

	list_destroy(list, nb_list);
	return 0;
}
