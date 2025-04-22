#pragma once
#include <stdint.h>

double test_tablegen_time(int repeat, void (*table_gen)());

int test_func(int block_len, void (*enc)(uint8_t *, uint8_t *), void (*dec)(uint8_t *, uint8_t *));

double test_enc_throughput(int block_len, int repeat, void (*enc)(uint8_t *, uint8_t *));