#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef uint64_t (*hash_func)(uint8_t *data, size_t len);

typedef struct {
    uint32_t size;  // power of 2
    hash_func hash_fn;
    hlist_head_t *heads;
} simple_ht_t;

typedef struct {
    hlist_node_t node;
    size_t key_len;
    void *key;
    void *value;
} simple_ht_entry_t;

static inline uint32_t __round_down_power_2(uint32_t v)
{
    uint32_t res = 1;
    while (v > 1) {
        v >>= 1;
        res <<= 1;
    }
    return res;
}

void *simple_ht_create(uint32_t hash_size, hash_func fn)
{
    hash_size = __round_down_power_2(hash_size);
    simple_ht_t *ht = malloc(sizeof(simple_ht_t) + sizeof(hlist_head_t)*hash_size);
    if (!ht) {
        return NULL;
    }
    *ht = (simple_ht_t) {
        .size = hash_size,
        .hash_fn = fn,
        .heads = (hlist_head_t *)(ht + 1),
    };
    // init hlist head
    memset(ht->heads, 0x0, sizeof(hlist_head_t) * hash_size);
    return ht;
}

void simple_ht_destroy(simple_ht_t *ht)
{
    uint32_t i;
    hlist_node_t *n;
    simple_ht_entry_t *ent;
    for (i = 0; i < ht->size; i++) {
        hlist_for_each_entry_safe(ent, n, &ht->heads[i], node) {
            // hlist_del(&ent->node);
            free(ent->key);
            free(ent->value);
            free(ent);
        }
        INIT_HLIST_HEAD(&ht->heads[i]);
    }
    free(ht);
}

bool simple_ht_insert(simple_ht_t *ht, void *key, size_t key_len, void *value)
{
    hlist_head_t *head = &ht->heads[ht->hash_fn((uint8_t *)key, key_len) & (ht->size-1)];
    simple_ht_entry_t *ent;

    // if (!hlist_empty(head)) {
    hlist_for_each_entry(ent, head, node) {
        if (ent->key_len == key_len && memcmp(ent->key, key, key_len) == 0) {
            free(ent->value);
            ent->value = value;
            return true;
        }
    }
    // not found, allocate
    ent = malloc(sizeof(simple_ht_entry_t));
    if (!ent) {
        return false;
    }
    *ent = (simple_ht_entry_t) {
        .node = {0},
        .key = key,
        .key_len = key_len,
        .value = value,
    };
    hlist_add_head(&ent->node, head);
    return true;
}

bool simple_ht_lookup(simple_ht_t *ht, const void *key, size_t key_len, void **value)
{
    hlist_head_t *head = &ht->heads[ht->hash_fn((uint8_t *)key, key_len) & (ht->size-1)];
    simple_ht_entry_t *ent;

    hlist_for_each_entry(ent, head, node) {
        if (ent->key_len == key_len && memcmp(ent->key, key, key_len) == 0) {
            *value = ent->value;
            return true;
        }
    }
    return false;
}

void simple_ht_remove(simple_ht_t *ht, const void *key, size_t key_len)
{
    hlist_head_t *head = &ht->heads[ht->hash_fn((uint8_t *)key, key_len) & (ht->size-1)];
    hlist_node_t *n;
    simple_ht_entry_t *ent;

    hlist_for_each_entry_safe(ent, n, head, node) {
        if (ent->key_len == key_len && memcmp(ent->key, key, key_len) == 0) {
            hlist_del(&ent->node);
            free(ent->key);
            free(ent->value);
            free(ent);
            return;
        }
    }
}
