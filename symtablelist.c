/*implementation of symtable using linked list*/

#include "symtable.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    const char *key;
    void *value;
    struct Node *next;
} Node;

struct SymTable_T {
    Node *first;
    size_t length;
};

SymTable_T SymTable_new(void) {
    SymTable_T table = malloc(sizeof(*table));
    if (table == NULL) return NULL;

    table->first = NULL;
    table->length = 0;
    return table;
}

void SymTable_free(SymTable_T oSymTable) {
    Node *current;
    Node *nextNode;

    assert(oSymTable != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        nextNode = current->next;
        free((char *)current->key);
        free(current);
        current = nextNode;
    }

    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);

    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    Node *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) return 1;
        current = current->next;
    }

    return 0;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
    Node *current;
    Node *newNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) return 0;
        current = current->next;
    }

    newNode = malloc(sizeof(*newNode));
    if (newNode == NULL) return 0;

    newNode->key = (char *)malloc(strlen(pcKey) + 1);
    if (newNode->key == NULL) {
        free(newNode);
        return 0;
    }

    strcpy((char *)newNode->key, pcKey);
    newNode->value = (void *)pvValue;
    newNode->next = oSymTable->first;
    oSymTable->first = newNode;
    oSymTable->length++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
    Node *current;
    void *oldValue;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            oldValue = current->value;
            current->value = (void *)pvValue;
            return oldValue;
        }
        current = current->next;
    }

    return NULL;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    Node *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) return current->value;
        current = current->next;
    }

    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    void *val;
    Node *prev;
    Node *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    prev = NULL;
    current = oSymTable->first;

    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            if (prev == NULL) {
                oSymTable->first = current->next;
            } else {
                prev->next = current->next;
            }
            val = current->value;
            free((char *)current->key);
            free(current);
            oSymTable->length--;
            return val;
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
    Node *current;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    current = oSymTable->first;
    while (current != NULL) {
        pfApply(current->key, current->value, (void *)pvExtra);
        current = current->next;
    }
}