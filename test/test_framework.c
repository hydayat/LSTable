/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-21 16:40:16
 * @LastEditTime: 2025-04-22 14:53:32
 * @Description: test framework
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_framework.h"

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

/**
 * @brief print an array byte by byte
 *
 * @param arr: byte array
 * @param len: length of array
 * @return * void
 */
void print_array(uint8_t *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

/**
 * @brief test the execution time of table generation algorithm
 *
 * @param repeat : number of repetitions
 * @param table_gen : table generation algorithm for test
 * @return double : execution time in seconds
 */
double test_tablegen_time(int repeat, void (*table_gen)())
{
    struct timespec start, end;
    double time_use;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repeat; i++)
    {
        table_gen();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    return time_sec(start, end) / repeat;
}

/**
 * @brief test functionality of encryption and decryption
 *
 * @param block_len : length of a block
 * @param enc : encryption algorithm
 * @param dec : decryption algorithm
 * @return int : 0 for fail and 1 for success
 */
int test_func(int block_len,
              void (*enc)(uint8_t *, uint8_t *),
              void (*dec)(uint8_t *, uint8_t *))
{
    // setup
    uint8_t *block = malloc(block_len);
    uint8_t *encrypted_block = malloc(block_len);
    uint8_t *decrypted_block = malloc(block_len);
    for (int i = 0; i < block_len; i++)
    {
        block[i] = rand();
    }
    // print_array(block, block_len);

    // encrypt
    enc(block, encrypted_block);
    // print_array(encrypted_block, block_len);

    // decrypt
    dec(encrypted_block, decrypted_block);
    // print_array(decrypted_block, block_len);

    // check
    int pass = 1;
    for (int i = 0; i < block_len; i++)
    {
        if (block[i] != decrypted_block[i])
        {
            // free
            free(block);
            free(encrypted_block);
            free(decrypted_block);
            return 0;
        }
    }
    // free
    free(block);
    free(encrypted_block);
    free(decrypted_block);
    return 1;
}

/**
 * @brief test the throughput of encryption algorithm
 *
 * @param block_len : length of a block
 * @param repeat : number of repetitions
 * @param enc : encryption algorithm
 * @return double : throughput (MB/s)
 */
double test_enc_throughput(int block_len, int repeat,
                           void (*enc)(uint8_t *, uint8_t *))
{
    // prepare plaintext
    size_t data_len = block_len * repeat;
    uint8_t *plaintext = malloc(data_len);
    uint8_t *ciphertext = malloc(data_len);
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    struct timespec start, end;
    double time_use;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repeat; i++)
    {
        enc(plaintext + i * block_len, ciphertext + i * block_len);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    int a = rand() % repeat;
    time_use = time_sec(start, end);
    return data_len / 1024 / 1024 / time_use;
}
