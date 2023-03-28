#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>


#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024

#define MAX_LINE 128
#define NEW_LINE '\n'


char identifiers[MAX_IDS][MAX_ID_LEN];
int lastId = 0;

const char *keywords[] = {
        "auto", "break", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if",
        "int", "long", "register", "return",
        "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while"
};

const int keywords_size = 32;

bool isBlank(char c) {
    return c == ' ' || c == '\t' || c == NEW_LINE;
}

bool isLetter(char c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95;
}


bool isKeyword(const char id[]) {
    for (int i = 0; i < keywords_size; ++i)
        if (strcmp(keywords[i], id) == 0) return true;
    return false;
}

bool isInIdentifiers(char id[]) {
    for (int i = 0; i < lastId; ++i)
        if (strcmp(identifiers[i], id) == 0) return true;
    return false;
}

void parseWord(char word[MAX_ID_LEN], int len) {
    if (isLetter(word[0])) {
        char id[MAX_ID_LEN];
        for (int i = 0; i < len; ++i) {
            if (word[i] == '(') break;
            id[i] = word[i];
        }
        if (isKeyword(id) || isInIdentifiers(id)) return;
        strcpy(identifiers[lastId++], id);
    }
}

int find_idents() {
    char string[MAX_LINE];
    while (fgets(string, MAX_LINE, stdin) != NULL) {
        bool during_word = false;
        bool during_comment = false;
        char word[MAX_ID_LEN];
        int lastLetter = 0;
        for (int i = 0; i < strlen(string); ++i) {
            char c = string[i];
            if (c == '/') {
                if (string[i + 1] == '*') during_comment = true;
                break;
            }
            if (c == '*' && string[i + 1] == '/') during_comment = false;
            if (!during_comment && during_word && isBlank(c)) {
                parseWord(word, lastLetter);
                lastLetter = 0;
                during_word = false;
            }
            if (!isBlank(c)) {
                during_word = true;
                word[lastLetter++] = c;
            }
        }
        if (!during_comment && during_word) {
            parseWord(word, lastLetter);
            during_word = false;
        }
    }

    return lastId;
}


int main(void) {
    printf("%d\n", find_idents());
    return 0;
}

