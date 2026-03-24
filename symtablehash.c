#include "symtable.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

enum {
    MAX_BUCKET_SIZE = 8
};

static const size_t bucket_sizes[MAX_BUCKET_SIZE] =
    {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

typedef struct Binding {
    const char *key;
    void *val;
    struct Binding *next;
} Binding;

struct SymTable_T {
    Binding **buckets;
    size_t length;
    int bucket_count_index;
};

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

    return uHash % uBucketCount;
}

static SymTable_T expand(SymTable_T oSymTable) {
    size_t oldLength;
    size_t newLength;
    Binding **newArray;
    size_t i;

    if (oSymTable->bucket_count_index + 1 >= MAX_BUCKET_SIZE)
        return oSymTable;

    oldLength = bucket_sizes[oSymTable->bucket_count_index];
    oSymTable->bucket_count_index++;

    newLength = bucket_sizes[oSymTable->bucket_count_index];
    newArray = calloc(newLength, sizeof(Binding *));
    if (newArray == NULL)
        return NULL;

    for (i = 0; i < oldLength; i++) {
        Binding *nextNew = NULL;
        Binding *currentOld = oSymTable->buckets[i];

        while (currentOld != NULL) {
            size_t index = SymTable_hash(currentOld->key, newLength);
            nextNew = newArray[index];
            newArray[index] = currentOld;
            currentOld = currentOld->next;
            newArray[index]->next = nextNew;
        }
    }

    free(oSymTable->buckets);
    oSymTable->buckets = newArray;
    return oSymTable;
}

SymTable_T SymTable_new(void) {
    SymTable_T table = calloc(1, sizeof(*table));
    if (table == NULL) return NULL;

    table->bucket_count_index = 0;
    table->buckets = calloc(bucket_sizes[table->bucket_count_index], sizeof(Binding *));

    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    table->length = 0;
    return table;
}

void SymTable_free(SymTable_T oSymTable) {
    size_t i;

    assert(oSymTable != NULL);

    for (i = 0; i < bucket_sizes[oSymTable->bucket_count_index]; i++) {
        Binding *current = oSymTable->buckets[i];
        Binding *next;

        while (current != NULL) {
            next = current->next;
            free((char *)current->key);
            free(current);
            current = next;
        }
    }

    free(oSymTable->buckets);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);

    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey,
        bucket_sizes[oSymTable->bucket_count_index]);

    current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0)
            return 1;
        current = current->next;
    }

    return 0;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue) {
    size_t index;
    Binding *current;
    Binding *newBinding;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey, bucket_sizes[oSymTable->bucket_count_index]);

    current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) return 0;
        current = current->next;
    }

    if (oSymTable->length >= bucket_sizes[oSymTable->bucket_count_index]) {
        oSymTable = expand(oSymTable);
        if (oSymTable == NULL) return 0;
    }

    index = SymTable_hash(pcKey, bucket_sizes[oSymTable->bucket_count_index]);

    newBinding = malloc(sizeof(Binding));
    if (newBinding == NULL) return 0;

    newBinding->key = malloc(strlen(pcKey) + 1);
    if (newBinding->key == NULL) {
        free(newBinding);
        return 0;
    }

    strcpy((char *) newBinding->key, pcKey);
    newBinding->val = (void *)pvValue;
    newBinding->next = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
    oSymTable->length++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue) {
    size_t index;
    void *value;
    Binding *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey,
        bucket_sizes[oSymTable->bucket_count_index]);

    current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            value = current->val;
            current->val = (void *)pvValue;
            return value;
        }
        current = current->next;
    }

    return NULL;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey,
        bucket_sizes[oSymTable->bucket_count_index]);

    current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) return current->val;
        current = current->next;
    }

    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    void *value;
    Binding *prev = NULL;
    Binding *current;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    index = SymTable_hash(pcKey,
        bucket_sizes[oSymTable->bucket_count_index]);

    current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, pcKey) == 0) {
            if (prev == NULL)
                oSymTable->buckets[index] = current->next;
            else
                prev->next = current->next;

            value = current->val;
            free((char *)current->key);
            free(current);
            oSymTable->length--;
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
    size_t i;

    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    for (i = 0; i < bucket_sizes[oSymTable->bucket_count_index]; i++) {
        Binding *current = oSymTable->buckets[i];
        while (current != NULL) {
            pfApply(current->key, current->val, (void *)pvExtra);
            current = current->next;
        }
    }
}