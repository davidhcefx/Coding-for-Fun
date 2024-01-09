#ifndef __SIMPLE_HT_H
#define __SIMPLE_HT_H
#ifdef __cplusplus
 extern "C"{
#endif
#include <stdint.h>
#include <stdbool.h>

/**
 * A simple hash table implemented by open-chaining with doubly linked-list.
 */
typedef void simple_ht_t;
typedef uint64_t (*hash_func)(uint8_t *data, size_t len);

/**
 * @param fn The hash function for calculating hash index.
 * @return NULL on failure
 */
simple_ht_t *simple_ht_create(uint32_t hash_size, hash_func fn);
void simple_ht_destroy(simple_ht_t *ht);
/**
 * @return false on internal failure
 * @note The key and value ownership now transferred to hash table.
 */
bool simple_ht_insert(simple_ht_t *ht, void *key, size_t key_len, void *value);
/**
 * @return true if found, otherwise false
 */
bool simple_ht_lookup(simple_ht_t *ht, const void *key, size_t key_len, void **value);
void simple_ht_remove(simple_ht_t *ht, const void *key, size_t key_len);

#ifdef __cplusplus
 }
#endif
#endif