/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-21 16:40:16
 * @LastEditTime: 2025-04-21 23:56:24
 * @Description: test framework
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#include <time.h>
#include <stdint.h>
#include "TableGen.h"

/**
 * @description: calculate the time elapsed
 * @param {struct timespec} start: start point
 * @param {struct timespec} end: end point
 * @return {double}: time elapsed (in seconds)
 */
double time_sec(struct timespec start, struct timespec end)
{
    return difftime(end.tv_sec, start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

void print_array(uint16_t *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

void test_lstable_tablegen_throughput(uint16_t (*sboxes)[1 << 16], uint16_t (*inv_sboxes)[1 << 16], int param_l, int repeat)
{
    struct timespec start, end;
    double time_use;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repeat; i++)
    {
        table_gen(sboxes, inv_sboxes, param_l);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_use = time_sec(start, end);
    printf("LSTable-(%d,R) table generation time(s): %2f\n", param_l, time_use / repeat);
}

void test_lstable_func(int param_l, int param_r,
                       void (*enc)(uint16_t *, uint16_t *, int),
                       void (*dec)(uint16_t *, uint16_t *, int))
{
    printf("Functionality test for LSTable-(%d, %d): \n", param_l, param_r);

    // setup
    uint16_t *block = malloc(2 * param_l);
    uint16_t *encrypted_block = malloc(2 * param_l);
    uint16_t *decrypted_block = malloc(2 * param_l);
    for (int i = 0; i < param_l; i++)
    {
        block[i] = rand();
    }
    print_array(block, param_l);

    // encrypt
    enc(block, encrypted_block, param_r);
    print_array(encrypted_block, param_l);

    // decrypt
    dec(encrypted_block, decrypted_block, param_r);
    print_array(decrypted_block, param_l);

    // check
    int pass = 1;
    for (int i = 0; i < param_l; i++)
    {
        if (block[i] != decrypted_block[i])
        {
            pass = 0;
            printf("Fail.\n");
            break;
        }
    }
    if (pass == 1)
    {
        printf("Success.\n");
    }

    // free
    free(block);
    free(encrypted_block);
    free(decrypted_block);
}

void test_lstable_enc_throughput(int param_l, int param_r_min, int param_r_max, int repeat,
                                 void (*enc)(uint16_t *, uint16_t *, int))
{
    // prepare plaintext
    size_t data_len = param_l * sizeof(uint16_t) * repeat;
    uint16_t *plaintext = malloc(data_len);
    uint16_t *ciphertext = malloc(data_len);
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    struct timespec start, end;
    double time_use;
    for (int param_r = param_r_min; param_r <= param_r_max; param_r += 2)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < repeat; i++)
        {
            enc(plaintext + i * param_l, ciphertext + i * param_l, param_r);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        int a = rand() % repeat;
        time_use = time_sec(start, end);
        double throughput = data_len / 1024 / 1024 / time_use;
        printf("LSTable-(%d,%d) throught(MB/s): %2f\n", param_l, param_r, throughput);
    }
}
