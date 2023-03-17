#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

enum state {
    OK = 0, UNDERFLOW = -1, OVERFLOW = -2
};

enum conflict_result {
    CONFLICT_A_WIN = 1, CONFLICT_B_WIN = -1, DRAW = 0
};

enum game_result {
    OUT_OF_CONFLICTS = 0, OUT_OF_CARDS = 1, GAME_WIN_A = 2, GAME_WIN_B = 3
};

enum version {
    STANDARD = 0, SIMPLIFIED = 1
};

#define PACK_SIZE 52
int MAX_CONFLICTS;
int pack[PACK_SIZE];
int stack_A[PACK_SIZE], stack_B[PACK_SIZE];
int first_A = 0, first_B = 0, len_A = 0, len_B = 0;

int table_stack_A[PACK_SIZE];
int table_stack_B[PACK_SIZE];
int table_len_A = 0, table_len_B = 0;

int rand_from_interval(int a, int b) {
    return rand() % (b - a + 1) + a;
}

void swap(int array[], int i, int k) {
    int tmp = array[i];
    array[i] = array[k];
    array[k] = tmp;
}

void shuffle_cards() {
    for (int i = 0; i <= PACK_SIZE - 1; i++) {
        pack[i] = i;
    }
    for (int i = 0; i <= PACK_SIZE - 2; i++) {
        int k = rand_from_interval(i, PACK_SIZE - 1);
        swap(pack, i, k);
    }
}

int add_card(int card_nr, int stack[], int first, int *len) {
    stack[(first + (*len)++) % PACK_SIZE] = card_nr;
    return OK;

}

int add_cards(int cards_to_add[], int number_of_cards_to_add, int stack[], int first, int *len) {
    for (int i = 0; i < number_of_cards_to_add; i++) {
        stack[(first + (*len)++) % PACK_SIZE] = cards_to_add[i];
    }
    return OK;
}

int get_card(int stack[], int *first, int *len) {
    if (*len == 0) return UNDERFLOW;
    int val = stack[(*first)++];
    *first %= PACK_SIZE;
    (*len)--;
    return val;
}


void deal_cards() {
    int i = 0;
    for (; i < PACK_SIZE / 2; i++) add_card(pack[i], stack_A, first_A, &len_A);
    for (; i < PACK_SIZE; i++) add_card(pack[i], stack_B, first_B, &len_B);
}

void init_game(void) {
    shuffle_cards();
    deal_cards();
}

int compare_cards(int card_A, int card_B) {
    int val_A = card_A / 4;
    int val_B = card_B / 4;
    if (val_A == val_B) return DRAW;
    if (val_A > val_B) return CONFLICT_A_WIN;
    return CONFLICT_B_WIN;
}

void print_stack(int stack[], int first, int len) {
    for (int i = 0; i < len; i++) printf("%d ", stack[(first + i) % PACK_SIZE]);
}

//te spacje są ohydne, ale tego wymagają testy
void print_result(int result, int conflicts) {
    printf("%d", result);
    switch (result) {
        case OUT_OF_CONFLICTS:
        case OUT_OF_CARDS: {
            printf(" %d %d", len_A, len_B);
            break;
        }
        case GAME_WIN_A: {
            printf(" %d", conflicts);
            break;
        }
        case GAME_WIN_B: {
            printf("\n");
            print_stack(stack_B, first_B, len_B);
            break;
        }
    }
}

int make_move() {
    int card_A = get_card(stack_A, &first_A, &len_A);
    int card_B = get_card(stack_B, &first_B, &len_B);

    if (card_A != UNDERFLOW) add_card(card_A, table_stack_A, 0, &table_len_A);
    if (card_B != UNDERFLOW) add_card(card_B, table_stack_B, 0, &table_len_B);
    if (card_A == UNDERFLOW || card_B == UNDERFLOW) {
        add_cards(table_stack_A, table_len_A, stack_A, first_A, &len_A);
        add_cards(table_stack_B, table_len_B, stack_B, first_B, &len_B);
    }
    if (card_A == UNDERFLOW || card_B == UNDERFLOW) {
        print_result(OUT_OF_CARDS, 0);
        exit(0);
    }

    return compare_cards(card_A, card_B);
}


void simplified_game(void) {
    const int ONLY_ONE_CARD = 1;
    for (int conflicts = 1; conflicts <= MAX_CONFLICTS; conflicts++) {
        int card_A = get_card(stack_A, &first_A, &len_A);
        int card_B = get_card(stack_B, &first_B, &len_B);

        int result = compare_cards(card_A, card_B);
        switch (result) {
            case CONFLICT_A_WIN: {
                add_card(card_A, stack_A, first_A, &len_A);
                add_card(card_B, stack_A, first_A, &len_A);

                if (len_B == 0) {
                    print_result(GAME_WIN_A, conflicts);
                    return;
                }
                break;
            }
            case CONFLICT_B_WIN: {
                add_card(card_B, stack_B, first_B, &len_B);
                add_card(card_A, stack_B, first_B, &len_B);

                if (len_A == 0) {
                    print_result(GAME_WIN_B, conflicts);
                    return;
                }
                break;
            }
            case DRAW: {
                add_card(card_A, stack_A, first_A, &len_A);
                add_card(card_B, stack_B, first_B, &len_B);
                break;
            }
        }
    }
    print_result(OUT_OF_CONFLICTS, 0);
}

void standard_game(void) {
    int conflicts = 0;

    while (conflicts < MAX_CONFLICTS) {

        int result;
        do {
            conflicts++;
            result = make_move();
            if (result == DRAW) make_move();
        } while (result == DRAW);

        switch (result) {
            case CONFLICT_A_WIN: {
                add_cards(table_stack_A, table_len_A, stack_A, first_A, &len_A);
                add_cards(table_stack_B, table_len_B, stack_A, first_A, &len_A);

                if (len_B == 0) {
                    print_result(GAME_WIN_A, conflicts);
                    return;
                }
                break;
            }
            case CONFLICT_B_WIN: {
                add_cards(table_stack_B, table_len_B, stack_B, first_B, &len_B);
                add_cards(table_stack_A, table_len_A, stack_B, first_B, &len_B);

                if (len_A == 0) {
                    print_result(GAME_WIN_B, conflicts);
                    return;
                }
                break;
            }
        }
        table_len_A = 0;
        table_len_B = 0;
    }
    print_result(OUT_OF_CONFLICTS, 0);
}


int main(void) {
    int seed, version;
    scanf("%d %d %d", &seed, &version, &MAX_CONFLICTS);
    srand((unsigned) seed);

    init_game();

    switch (version) {
        case STANDARD:
            standard_game();
            break;
        case SIMPLIFIED:
            simplified_game();
            break;
        default:
            printf("NOTHING TO DO!\n");
    }
    return 0;
}
