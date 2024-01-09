#include <stdio.h>
#include <string.h>
#include "simple_ht.h"

#define HASH_TABLE_SIZE  32*1024*1024  // 32MB

uint64_t __my_hash(uint8_t *data, size_t len)
{
    uint64_t res = 0;
    for ( ; len > 0; len--) {
        res += *data;
        data++;
    }
    return res;
}

int main() {
    simple_ht_t *simp_ht;
    char *value;
    bool ret = true;

    simp_ht = simple_ht_create(HASH_TABLE_SIZE, __my_hash);
    if (!simp_ht) {
        fprintf(stderr, "Error: simple_ht_create failed\n");
        return -1;
    }
    ret &= simple_ht_insert(simp_ht, strdup("hello"), 5, strdup("hello value"));
    ret &= simple_ht_insert(simp_ht, strdup("world"), 5, strdup("world value"));
    ret &= simple_ht_insert(simp_ht, strdup("123"), 3, strdup("123 value"));
    if (!ret) {
        return -1;
    }
    if (simple_ht_lookup(simp_ht, "world", 5, (void **)&value)) {
        printf("world: %s\n", value);
    }
    if (simple_ht_lookup(simp_ht, "hel", 3, (void **)&value)) {
        printf("hel: %s\n", value);
    }
    if (simple_ht_lookup(simp_ht, "hello", 5, (void **)&value)) {
        printf("hello: %s\n", value);
    }

    simple_ht_destroy(simp_ht);

    return 0;
}
