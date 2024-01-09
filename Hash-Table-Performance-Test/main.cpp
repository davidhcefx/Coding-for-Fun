#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include "simple_ht.h"

#define HASH_TABLE_SIZE  4*1024*1024  // 4M
#define SAMPLE_KEY       "someLongStringWithMeaninglessContent"
#define INPUT_KEY_LEN    20  // should be shorter than SAMPLE_KEY, but larger than strlen(HASH_TABLE_SIZE)
#define INPUT_NB_NOT_FOUND    1024  // among the inputs, how many should test the not-found case
#define VERIFY_LOOKUP_RESULT  false

void *g_input[HASH_TABLE_SIZE][2];

static uint64_t __my_hash(uint8_t *data, size_t len)
{
    // TODO: this hash algo is not evenly distributed, which hurts performance
    uint64_t res = 0;
    while (len > 8) {
        res += *(uint64_t *)data;
        data += 8;
        len -= 8;
    }
    while (len > 0) {
        res += *data;
        data++;
        len--;
    }
    return res;
}

static inline uint64_t gettime_ms() {
    struct timespec ts;
    // assert(clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0);
    assert(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

void test_simple_ht(void *input[HASH_TABLE_SIZE][2], float load_factor)
{
    simple_ht_t *ht;
    void *val;
    int nb = (int)(HASH_TABLE_SIZE * load_factor);
    int i;
    uint64_t start_ms, insert_ms, lookup_ms;

    printf("%s (load factor %.2f)\n", __func__, load_factor);
    assert(ht = simple_ht_create(HASH_TABLE_SIZE, __my_hash));

    start_ms = gettime_ms();
    for (i = 0; i < nb; i++) {
        assert(simple_ht_insert(ht, input[i][0], INPUT_KEY_LEN, input[i][1]));
    }
    insert_ms = gettime_ms() - start_ms;
    printf("  insert: %lu ms, %lu ns/op\n", insert_ms, insert_ms * 1000 / nb);

    start_ms = gettime_ms();
    for (i = 0; i < nb - INPUT_NB_NOT_FOUND; i++) {
        assert(simple_ht_lookup(ht, input[i][0], INPUT_KEY_LEN, &val));
#if VERIFY_LOOKUP_RESULT
        assert(val && strcmp((char *)input[i][1], (char *)val) == 0);
#endif
    }
    for (i = nb - INPUT_NB_NOT_FOUND; i < nb; i++) {
        assert(!simple_ht_lookup(ht, input[i][0], INPUT_KEY_LEN - 2, &val));
    }
    lookup_ms = gettime_ms() - start_ms;
    printf("  lookup: %lu ms, %lu ns/op (%d found, %d not found)\n", lookup_ms, lookup_ms * 1000 / nb,
            nb - INPUT_NB_NOT_FOUND, INPUT_NB_NOT_FOUND);

    simple_ht_destroy(ht);
}

int main()
{
    int i;
    char *key;
    char buf[32];

    assert(snprintf(buf, sizeof(buf), "%d", HASH_TABLE_SIZE) <= INPUT_KEY_LEN);
    assert(INPUT_KEY_LEN < strlen(SAMPLE_KEY));

    // prepare input
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
        assert(key = (char *)malloc(INPUT_KEY_LEN));
        snprintf(key, INPUT_KEY_LEN, "%d%s", i, SAMPLE_KEY);
        g_input[i][0] = key;
        g_input[i][1] = strdup(key);
    }

    test_simple_ht(g_input, 0.25);
    test_simple_ht(g_input, 0.50);
    test_simple_ht(g_input, 0.75);
    test_simple_ht(g_input, 0.90);

    return 0;
}
