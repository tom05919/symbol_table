/*header file for symtable, lists all avaliable funcitons*/

#include <stddef.h>

/* Opaque handle to a symbol table object. */
typedef struct SymTable_T *SymTable_T;

/*
 * Create and return a new empty symbol table.
 * Return NULL if insufficient memory is available.
 */
SymTable_T SymTable_new(void);

/*
 * Free all memory associated with oSymTable.
 * The client is responsible for freeing any values stored in the table.
 */
void SymTable_free(SymTable_T oSymTable);

/*
 * Return the number of bindings currently stored in oSymTable.
 */
size_t SymTable_getLength(SymTable_T oSymTable);

/*
 * Add a new binding from pcKey to pvValue.
 * Return 1 if the binding is added successfully.
 * Return 0 if pcKey is already present or if insufficient memory exists.
 */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/*
 * Replace the value associated with pcKey by pvValue.
 * Return the old value if pcKey is present.
 * Return NULL if pcKey is not present.
 */
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/*
 * Return 1 if pcKey is a key in oSymTable.
 * Otherwise return 0.
 */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*
 * Return the value associated with pcKey.
 * Return NULL if pcKey is not present.
 */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*
 * Remove the binding with key pcKey from oSymTable.
 * Return the associated value if pcKey is present.
 * Return NULL if pcKey is not present.
 */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*
 * Apply pfApply to each binding in oSymTable.
 * For each binding, pfApply is called with the key, value,
 * and pvExtra.
 */
void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);