#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef uint64_t (*hash_cb)(uint8_t *data, size_t len);

typedef struct {
    uint32_t size;  // power of 2
    hash_cb hash_fn;
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

simple_ht_t *simple_ht_create(uint32_t hash_size, hash_cb fn)
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
        // INIT_HLIST_HEAD(&ht->heads[i]);
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
        .node = {NULL, NULL},
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

void simple_ht_bucket_stats(simple_ht_t *ht, uint32_t *stats, uint32_t nb_stats)
{
    uint32_t i;
    simple_ht_entry_t *ent;
    uint32_t n = nb_stats - 2;

    if (nb_stats < 2) {
        return;
    }
    memset(stats, 0, nb_stats);
    for (i = 0; i < ht->size; i++) {
        uint32_t count = 0;
        hlist_for_each_entry(ent, &ht->heads[i], node) {
            count++;
        }
        if (count <= n) {
            stats[count]++;
        } else {
            stats[nb_stats - 1]++;
        }
    }
}