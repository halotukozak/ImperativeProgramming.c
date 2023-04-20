#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
    int first;
    int second;
} pair;

pair construct_pair(int first, int second) {
    pair p;
    p.first = first;
    p.second = second;
    return p;
}

// Comparator for pair
int cmp_pair(const void *a, const void *b) {
    const pair *a_ = (const pair *) a;
    const pair *b_ = (const pair *) b;
    return (a_->first == b_->first && a_->second == b_->second);
}

// Check presence of value in the tab
int in_array(const int *tab, int n, int val) {
    for (int i = 0; i < n; ++i) if (tab[i] == val) return 1;
    return 0;
}

int cmp_func(const void *a, const void *b) {
    int arg1 = *(const int *) a;
    int arg2 = *(const int *) b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

// Get domain of relation
int get_domain(const pair *relation, int n, int *domain) {
    int last = 0;
    for (int i = 0; i < n; ++i) {
        pair p = relation[i];
        if (!in_array(domain, last, p.first)) domain[last++] = p.first;
        if (!in_array(domain, last, p.second)) domain[last++] = p.second;
    }
    qsort(domain, last, sizeof(int), cmp_func);
    return last;
}


// Add pair to existing relation if not already there
int add_relation(pair *tab, int n, pair new_pair, int *last) {
    for (int i = 0; i < n; i++) if (cmp_pair(&tab[i], &new_pair)) return 0;
    tab[(*last)++] = new_pair;
    return 1;
}

// Read number of pairs, n, and then n pairs of ints
int read_relation(pair *relation) {
    int n;
    int last = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        pair newPair;
        scanf("%d %d", &newPair.first, &newPair.second);
        add_relation(relation, n, newPair, &last);
    }
    return n;
}

void print_int_array(const int *array, int n) {
    printf("\n%d\n", n);
    for (int i = 0; i < n; ++i) printf("%d ", array[i]);
}
// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(const pair *relation, int n) {
    int domain[MAX_REL_SIZE];
    int last = get_domain(relation, n, domain);
    int isReflexive;
    for (int i = 0; i < last; ++i) {
        isReflexive = 0;
        int x = domain[i];
        pair xx = construct_pair(x, x);
        for (int j = 0; j < n; ++j) {
            if (cmp_pair(&xx, &relation[j])) {
                isReflexive = 1;
                break;
            }
        }
        if (!(isReflexive)) return 0;
    }

    return 1;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(const pair *relation, int n) {
    int domain[MAX_REL_SIZE];
    int last = get_domain(relation, n, domain);
    for (int i = 0; i < last; ++i) {
        int x = domain[i];
        pair xx = construct_pair(x, x);
        for (int j = 0; j < n; ++j) if (cmp_pair(&xx, &relation[j])) return 0;
    }
    return 1;
}

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(const pair *relation, int n) {
    int isSymmetric;
    for (int i = 0; i < n; ++i) {
        isSymmetric = 0;
        int x = relation[i].first, y = relation[i].second;
        pair yx = construct_pair(y, x);
        for (int j = 0; j < n; ++j)
            if (cmp_pair(&yx, &relation[j])) {
                isSymmetric = 1;
                break;
            }

        if (!isSymmetric) return 0;
    }

    return 1;
}

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(const pair *relation, int n) {
    for (int i = 0; i < n; ++i) {
        pair xy = relation[i];
        int x = xy.first, y = xy.second;
        pair yx = construct_pair(y, x);
        for (int j = 0; j < n; ++j)
            if (x != y && cmp_pair(&xy, &yx)) return 0;
    }
    return 1;
}

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(const pair *relation, int n) {
    for (int i = 0; i < n; ++i) {
        pair yx = construct_pair(relation[i].second, relation[i].first);
        for (int j = 0; j < n; ++j) if (cmp_pair(&yx, &relation[j])) return 0;
    }
    return 1;
}

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(const pair *relation, int n) {
    for (int i = 0; i < n; ++i) {
        int x = relation[i].first;
        int y = relation[i].second;
        for (int j = 0; j < n; ++j) {
            if (relation[j].first == y) {
                int z = relation[j].second;
                pair xz = construct_pair(x, z);
                int isTransitive = 0;
                for (int k = 0; k < n; ++k) {
                    if (cmp_pair(&xz, &relation[k])) {
                        isTransitive = 1;
                        break;
                    }
                }
                if (!isTransitive) return 0;
            }
        }
    }
    return 1;
}
// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(const pair *relation, int n) {
    return is_reflexive(relation, n) && is_antisymmetric(relation, n) && is_transitive(relation, n);
}

// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
int is_connected(const pair *relation, int n) {
    int domain[MAX_REL_SIZE];
    int last = get_domain(relation, n, domain);
    int is_connected;
    for (int i = 0; i < last; ++i) {
        for (int j = 0; j < last; ++j) {
            is_connected = 0;
            int x = domain[i];
            int y = domain[j];
            pair xy = construct_pair(x, y);
            pair yx = construct_pair(y, x);

            for (int k = 0; k < n; ++k) {
                if (cmp_pair(&relation[k], &xy) || cmp_pair(&relation[k], &yx)) {
                    is_connected = 1;
                    break;
                }
            }
            if (!is_connected) return 0;
        }
    }
    return 1;
}

// A total order relation is a partial order relation that is connected
int is_total_order(const pair *relation, int n) {
    return is_partial_order(relation, n) && is_connected(relation, n);
}


int find_max_elements(const pair *relation, int n, int *max_elements) {
    int domain[MAX_REL_SIZE];
    int last = get_domain(relation, n, domain);
    int i = 0;
    int is_max;
    for (int j = 0; j < last; ++j) {
        int x = domain[j];
        is_max = 1;
        for (int k = 0; k < n; ++k) {
            if (relation[k].second != x && relation[k].first == x) {
                is_max = 0;
                break;
            }
        }

        if (is_max) max_elements[i++] = x;
    }
    qsort(max_elements, i, sizeof(int), cmp_func);

    return i;
}


int find_min_elements(const pair *relation, int n, int *min_elements) {
    int domain[MAX_REL_SIZE];
    int last = get_domain(relation, n, domain);
    int i = 0;
    int is_max;
    for (int j = 0; j < last; ++j) {
        int x = domain[j];
        is_max = 1;
        for (int k = 0; k < n; ++k) {
            if (relation[k].first != x && relation[k].second == x) {
                is_max = 0;
                break;
            }
        }

        if (is_max) min_elements[i++] = x;
    }
    qsort(min_elements, i, sizeof(int), cmp_func);
    return i;
}


// Case 3:

int composition(const pair *relation1, int n, const pair *relation2, int m, pair *compRelation) {
    int last = 0;
    for (int i = 0; i < n; ++i) {
        int x = relation1[i].first;
        int y = relation1[i].second;
        for (int j = 0; j < n; ++j) {
            if (relation2[j].first == y) {
                int z = relation2[j].second;
                pair xz = construct_pair(x, z);
                add_relation(compRelation, MAX_REL_SIZE, xz, &last);
            }
        }
    }
    return last;
}


int main(void) {
    int to_do;
    pair relation[MAX_REL_SIZE];
    pair relation_2[MAX_REL_SIZE];
    pair comp_relation[MAX_REL_SIZE];
    int domain[MAX_REL_SIZE];
    int max_elements[MAX_REL_SIZE];
    int min_elements[MAX_REL_SIZE];

    scanf("%d", &to_do);
    int size = read_relation(relation);
    int ordered, size_2, n_domain;

    switch (to_do) {
        case 1:
            printf("%d ", is_reflexive(relation, size));
            printf("%d ", is_irreflexive(relation, size));
            printf("%d ", is_symmetric(relation, size));
            printf("%d ", is_antisymmetric(relation, size));
            printf("%d ", is_asymmetric(relation, size));
            printf("%d\n", is_transitive(relation, size));
            break;
        case 2:
            ordered = is_partial_order(relation, size);
            n_domain = get_domain(relation, size, domain);
            printf("%d %d", ordered, is_total_order(relation, size));
            print_int_array(domain, n_domain);
            if (!ordered) break;
            int no_max_elements = find_max_elements(relation, size, max_elements);
            int no_min_elements = find_min_elements(relation, size, min_elements);
            print_int_array(max_elements, no_max_elements);
            print_int_array(min_elements, no_min_elements);
            break;
        case 3:
            size_2 = read_relation(relation_2);
            printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}

