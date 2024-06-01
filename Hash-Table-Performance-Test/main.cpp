#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include "jhash.h"
#include "simple_ht.h"

#define HASH_TABLE_SIZE           4*1024*1024  // 4M
#define INPUT_MAX_LOAD_FACTOR     2.0
#define INPUT_KEY_LEN             20  // should be longer than str(HASH_TABLE_SIZE*INPUT_MAX_LOAD_FACTOR)
#define SAMPLE_KEY                "someLongStringWithMeaninglessContent"
#define INPUT_MAX_NB              (size_t)((HASH_TABLE_SIZE)*(INPUT_MAX_LOAD_FACTOR))
#define INPUT_NB_NOT_FOUND        1024LU  // among the inputs, how many should test the not-found case
#define DEBUG_CHECK_LOOKUP_VALUE  false
#define DEBUG_COUNT_BKT_STATS     false

char g_input_key[INPUT_MAX_NB][INPUT_KEY_LEN];
char g_input_value[INPUT_MAX_NB][INPUT_KEY_LEN];

static uint64_t __hash_cb(uint8_t *data, size_t len)
{
    // uint64_t res = 0;
    // while (len > 8) {
    //     res += *(uint64_t *)data;
    //     data += 8;
    //     len -= 8;
    // }
    // while (len > 0) {
    //     res += *data;
    //     data++;
    //     len--;
    // }
    // return res;

    return jhash(data, len, 0);
}

static inline uint64_t gettime_ms()
{
    struct timespec ts;
    // assert(clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0);
    assert(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

void test_simple_ht(float load_factor)
{
    simple_ht_t *ht;
    size_t nb = (size_t)(load_factor * HASH_TABLE_SIZE);
    void **keys, **values;
    uint64_t start_ms, diff;
    uint32_t stats[5] __attribute__((unused));
    void *val;
    size_t i, k;

    // prepare input
    assert(keys = (void **)malloc(nb * sizeof(void *)));
    assert(values = (void **)malloc(nb * sizeof(void *)));
    for (i = 0; i < nb; i++) {
        assert(keys[i] = strdup(g_input_key[i]));
        assert(values[i] = strdup(g_input_value[i]));
    }

    printf("%s (load factor %.2f)\n", __func__, load_factor);
    assert(ht = simple_ht_create(HASH_TABLE_SIZE, __hash_cb));

    // insert
    start_ms = gettime_ms();
    for (i = 0; i < nb; i++) {
        assert(simple_ht_insert(ht, keys[i], INPUT_KEY_LEN, values[i]));
    }
    diff = gettime_ms() - start_ms;
    printf("  insert: %lu ms, %lu ns/op\n", diff, diff * 1000000 / nb);

    // lookup
    start_ms = gettime_ms();
    for (k = 0; k < 5; k++) {  // repeat 5 times
        for (i = 0; i < nb - INPUT_NB_NOT_FOUND; i++) {
            assert(simple_ht_lookup(ht, keys[i], INPUT_KEY_LEN, &val));
#if DEBUG_CHECK_LOOKUP_VALUE
            assert(val && strcmp((char *)values[i], (char *)val) == 0);
#endif
        }
        for (i = nb - INPUT_NB_NOT_FOUND; i < nb; i++) {
            assert(!simple_ht_lookup(ht, keys[i], INPUT_KEY_LEN - 2, &val));
        }
    }
    diff = gettime_ms() - start_ms;
    printf("  lookup: %lu ms, %lu ns/op\n", diff, diff * 1000000 / nb);

#if DEBUG_COUNT_BKT_STATS
    printf("  bucket stats: ");
    simple_ht_bucket_stats(ht, stats, sizeof(stats)/sizeof(uint32_t));
    for (i = 0; i < sizeof(stats)/sizeof(uint32_t); i++) {
        printf("%u, ", stats[i]);
    }
    printf("\n");
#endif
    simple_ht_destroy(ht);
    free(keys);
    free(values);
}

int main()
{
    char buf[32];
    size_t i;

    snprintf(buf, sizeof(buf), "%lu", INPUT_MAX_NB);
    assert(INPUT_KEY_LEN >= strlen(buf));

    // prepare input
    for (i = 0; i < INPUT_MAX_NB; i++) {
        snprintf(g_input_key[i], INPUT_KEY_LEN, "%lu%s", i, SAMPLE_KEY);
        snprintf(g_input_value[i], INPUT_KEY_LEN, "%lu-v", i);
    }

    test_simple_ht(0.25);
    test_simple_ht(0.50);
    test_simple_ht(0.75);
    test_simple_ht(1.00);
    test_simple_ht(2.00);

    return 0;
}
