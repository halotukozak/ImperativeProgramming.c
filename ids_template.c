#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024

#define MAX_LINE 128
#define BLANK_CHARS 16


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

bool isValidCharacter(char c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95 || c == 35;
}

bool isValidFirstCharacter(char c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95 || c == 35;
}

bool isNotKeyword(const char id[]) {
    for (int i = 0; i < keywords_size; ++i)
        if (strcmp(keywords[i], id) == 0) return false;
    return true;
}

bool isNotInIdentifiers(const char id[]) {
    for (int i = 0; i < lastId; ++i)
        if (strcmp(identifiers[i], id) == 0) return false;
    return true;
}

void parseWord(const char word[MAX_ID_LEN]) {
    if (isValidFirstCharacter(word[0]) && isNotKeyword(word) & isNotInIdentifiers(word))
        strcpy(identifiers[lastId++], word);
}

int find_idents() {
    char string[MAX_LINE];
    bool during_word = false;
    bool omit = false;
    bool during_multiline_comment = false;
    bool during_string = false;

    char string_char = '"';
    while (fgets(string, MAX_LINE, stdin) != NULL) {
        char word[MAX_ID_LEN] = {0};
        int lastLetter = 0;
        size_t len = strlen(string);
        for (int i = 0; i < len; ++i) {
            char c = string[i];

            if (!during_multiline_comment) {
                // comments
                if (i + 1 < len) {
                    // start of the multiline comment
                    if (c == '/' && string[i + 1] == '*') {
                        during_multiline_comment = true;
                        omit = true;
                        i += 1;
                    }
                        // one-line comment
                    else if (c == '/' && string[i + 1] == '/') break;
                }

                if (!omit) {
                    // start of the string
                    if (i > 0 && (c == '"' || c == '\'') && string[i - 1] != '\\') {
                        string_char = c;
                        during_string = true;
                        omit = true;
                    }

                    // start of the word
                    if (isValidFirstCharacter(c)) {
                        during_word = true;
                        word[lastLetter++] = c;
                    } else if (during_word && isValidCharacter(c)) {
                        word[lastLetter++] = c;
                    }

                    // end of the word
                    if (during_word && !isValidCharacter(c)) {
                        parseWord(word);
                        lastLetter = 0;
                        memset(word, 0, MAX_ID_LEN);
                        during_word = false;
                        omit = false;
                    }
                } else if (during_string && c == string_char) {
                    if ((i > 0 && string[i - 1] != '\\') ||
                        (i > 1 && (string[i - 1] != '\\' || (string[i - 1] == '\\' && string[i - 2] == '\\')))) {

                        omit = false;
                        during_string = false;
                    }
                }
            } else if (i + 1 < len && c == '*' && string[i + 1] == '/') {
                during_multiline_comment = false;
                omit = false;
                i += 1;
            }
        }
    }

//    for (int i = 0; i < lastId; ++i) printf("%s\n", identifiers[i]);
    return lastId;
}

int main(void) {
    printf("%d\n", find_idents());
    return 0;
}
