#include "symtable.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

enum {
    BUCKET_SIZES_LENGTH = 8
};

size_t bucket_sizes[8] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};
size_t *size = bucket_sizes;
size_t *end = bucket_sizes + BUCKET_SIZES_LENGTH;

typedef struct Binding {
    const char *key;
    void *val;
    struct Binding *next;
} Binding;

typedef struct SymTable_T {
    struct Binding **buckets;
    int length;
};

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

static SymTable_T expand(SymTable_T oSymTable, size_t newLength) {
    SymTable_T oldSym = oSymTable;
    Binding **newArray = calloc(newLength, sizeof(Binding *));

    if (newArray == NULL) return NULL;

    int i;
    for (i = 0; i < *size; i++) {
        Binding *currentOld = oSymTable->buckets[i];
        Binding *currentNew = newArray[i];
        Binding *prev = NULL;
        while(currentOld != NULL) {
            currentNew->key = malloc(strlen(currentOld->key) + 1);
            if (currentNew->key == NULL) return NULL;

            strcpy(currentNew->key, currentOld->key);
            currentNew->val = currentOld->val;
            
        }
    }
    free(oSymTable);

}

SymTable_T SymTable_new(void) {
    int i;
    SymTable_T table = calloc(1, sizeof(*table));
    if (table == NULL) return NULL;

    table->buckets = calloc(*size, sizeof(Binding *));
    if (table->buckets == NULL) return NULL;

    table->length = 0;
    return table;
}

void SymTable_free(SymTable_T oSymTable) {
    int i;
    for (i = 0; i < *size; i++) {
        Binding *current = oSymTable->buckets[i];
        Binding *next;
        while(current != NULL) {
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t index = SymTable_hash(pcKey, *size);

    Binding *current = oSymTable->buckets[index];
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

    if(oSymTable->length >= *size) {
        if (size != end) size++;
        oSymTable = expand(oSymTable, *size);
    }
    
    size_t index = SymTable_hash(pcKey, *size);
    if (oSymTable->buckets[index] == NULL) oSymTable->length += 1;

    Binding *newBinding = malloc(sizeof(Binding));
    if (newBinding = NULL) return 0;

    newBinding->key = malloc(strlen(pcKey) + 1);
    if (newBinding->key == NULL) return 0;

    strcpy(newBinding->key, pcKey);
    newBinding->val = pvValue;
    newBinding->next = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
    return 1; 
}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {

    size_t index = SymTable_hash(pcKey, *size);
    void *value; 

    Binding *current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            value = current->val;
            current->val = pvValue;
            return value;
        }
        current = current->next;
    }

    return NULL;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t index = SymTable_hash(pcKey, *size);

    Binding *current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            return current->val;
        }
        current = current->next;
    }
    return 0;

}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t index = SymTable_hash(pcKey, *size);
    void *value;
    Binding *prev = NULL;
    Binding *current = oSymTable->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            if (prev == NULL) {
                oSymTable->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            value = current->val;
            free(current->key);
            free(current);
            return value;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {

    int i;
    for (i = 0; i < *size; i++) {
        Binding *current = oSymTable->buckets[i];
        while(current != NULL) {
            pfApply(current->key, current->val, pvExtra);
            current = current->next;
        }
    }
}