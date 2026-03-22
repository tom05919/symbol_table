#include "symtable.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct Node {
    const char *key;
    void *value;
    struct Node *next;
} Node;

typedef struct SymTable_T {
    struct Node *first;
    int length;
};

SymTable_T SymTable_new(void) {
    SymTable_T table = malloc(sizeof(*table));
    if (table == NULL) return NULL;
    table->first = NULL;
    table->length = 0;
    return table;
}

void SymTable_free(SymTable_T oSymTable) {
    Node *current = oSymTable->first; 
    Node *nextNode;
    while(current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    Node *current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
    
    if (SymTable_contains(oSymTable, pcKey) == 1) return 0;
    
    Node *prev = oSymTable->first;
    Node *new = malloc(sizeof(Node));
    if (new == NULL) {
        return 0; 
    }
    new->key = (const char*)malloc(strlen(pcKey) + 1);
    if (new->key == NULL) {
        free(new);
        return 0;
    }
    strcpy((char*)new->key, pcKey);
    new->value = pvValue;
    new->next = prev;
    oSymTable->first = new;
    oSymTable->length += 1;
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {

    Node *current = oSymTable->first;
    void *oldValue;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            oldValue = current->value;
            current->value = pvValue;
            return oldValue;
        } else {
            current = current->next;
        }
    }

    return NULL;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    Node *current = oSymTable->first;
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            return current->key;
        }
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    void *val;
    Node *prev = NULL;
    Node *current = oSymTable->first;

    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            if (prev == NULL) {
                oSymTable->first = current->next;
            } else {
                prev->next = current->next;
            }
            val = current->value;
            free(current->key);
            free(current);
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
        
    Node *current = oSymTable->first;
    while (current != NULL) {
        (*pfApply)(current->key, current->value, pvExtra);
        current = current->next;
    }
}