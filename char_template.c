#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FIRST_CHAR 33
#define LAST_CHAR 127
#define BIGRAM_SIZE (LAST_CHAR - FIRST_CHAR) * 100 + LAST_CHAR - FIRST_CHAR
#define MAX_LINE 128

#define NEWLINE '\n'

bool isBlank(char c) {
    return c == ' ' || c == '\t' || c == NEWLINE;
}

// count lines, words & chars in size given text file
void wc(int *nl, int *nw, int *nc) {
    int lines = 0, words = 0, chars = 0;
    char string[MAX_LINE];
    while (fgets(string, MAX_LINE, stdin) != NULL) {
        lines++;
        bool during_word = false;
        for (int i = 0; i < strlen(string); ++i) {
            char c = string[i];
            if (during_word && isBlank(c)) {
                during_word = false;
                words++;
            }
            if (!isBlank(c))
                during_word = true;
        }
        if (during_word) {
            words++;
            during_word = false;
        }
        chars += strlen(string);
    }

    *nl = lines;
    *nw = words;
    *nc = chars;

}

void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void char_count(int char_no, int *n_char, int *cnt) {
    int counter[LAST_CHAR - FIRST_CHAR][2] = {0};
    char string[MAX_LINE];
    while (fgets(string, MAX_LINE, stdin) != NULL) {
        for (int i = 0; i < strlen(string); ++i) {
            if (string[i] == '\0') break;
            char c = string[i];
            if (!isBlank(c)) {
                counter[c - FIRST_CHAR][0] = c;
                counter[c - FIRST_CHAR][1] += 1;
            }
        }
    }

    for (int i = 0; i < LAST_CHAR - FIRST_CHAR - 1; i++)
        for (int j = 0; j < LAST_CHAR - FIRST_CHAR - i - 1; j++)
            if (counter[j][1] < counter[j + 1][1])
                for (int k = 0; k < 2; ++k) swap(&counter[j][k], &counter[j + 1][k]);

    *n_char = counter[char_no - 1][0];
    *cnt = counter[char_no - 1][1];

}


void bigram_count(int bigram_no, int bigram[]) {
    int counter[BIGRAM_SIZE][3] = {0};
    char string[MAX_LINE];
    while (fgets(string, MAX_LINE, stdin) != NULL) {
        for (int i = 0; i < strlen(string) - 1; ++i) {
            if (string[i] == '\0') break;
            char c1 = string[i];
            char c2 = string[i + 1];

            if (!isBlank(c1) && !isBlank(c2)) {
                int index = ((int) c1 - FIRST_CHAR) * 100 + (int) c2 - FIRST_CHAR;
                counter[index][0] = c1;
                counter[index][1] = c2;
                counter[index][2] += 1;
            }
        }
    }

    for (int i = 0; i < BIGRAM_SIZE - 1; i++)
        for (int j = 0; j < BIGRAM_SIZE - i - 1; j++)
            if (counter[j][2] < counter[j + 1][2])
                for (int k = 0; k < 3; ++k) swap(&counter[j][k], &counter[j + 1][k]);

    for (int k = 0; k < 3; ++k) bigram[k] = counter[bigram_no - 1][k];
}

void find_comments(int *line_comment_counter, int *block_comment_counter) {
    int line_comments = 0;
    int block_comments = 0;
    char string[MAX_LINE];

    bool isOpen = false;

    while (fgets(string, MAX_LINE, stdin) != NULL) {
        for (int i = 0; i < strlen(string) - 1; ++i) {
            if (string[i] == '\0') break;
            char c1 = string[i];
            char c2 = string[i + 1];

            if (!isOpen && c1 == '/' && c2 == '/') {
                line_comments++;
                break;
            }

            if (!isOpen && c1 == '/' && c2 == '*') {
                isOpen = true;
                i++;
            }
            if (isOpen && c1 == '*' && c2 == '/') {
                block_comments++;
                isOpen = false;
            }

        }
    }

    *line_comment_counter = line_comments;
    *block_comment_counter = block_comments;
}


int read_int() {
    char line[MAX_LINE];
    fgets(line, MAX_LINE, stdin); // to get the whole line
    return (int) strtol(line, NULL, 10);
}

int main(void) {
    int to_do;
    int nl, nw, nc, char_no, n_char, cnt;
    int line_comment_counter, block_comment_counter;
    int bigram[3];

    to_do = read_int();
    switch (to_do) {
        case 1: // wc()
            wc(&nl, &nw, &nc);
            printf("%d %d %d\n", nl, nw, nc);
            break;
        case 2: // char_count()
            char_no = read_int();
            char_count(char_no, &n_char, &cnt);
            printf("%c %d\n", n_char, cnt);
            break;
        case 3: // bigram_count()
            char_no = read_int();
            bigram_count(char_no, bigram);
            printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
            break;
        case 4:
            find_comments(&line_comment_counter, &block_comment_counter);
            printf("%d %d\n", block_comment_counter, line_comment_counter);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

