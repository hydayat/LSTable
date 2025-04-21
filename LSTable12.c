/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:40:27
 * @LastEditTime: 2025-04-21 21:56:38
 * @Description: Implementation and Test Cases of LSTable-(12,R).
 *
 * Copyright (c) 2024 by Tongji University, All Rights Reserved.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TableGen.h"
#include "TestFramework.h"

// lookup table
uint16_t sboxes[12][1 << 16];     // for encryption
uint16_t inv_sboxes[12][1 << 16]; // for decryption

void confuse(uint16_t *in, uint16_t boxes[12][1 << 16])
{
    for (int i = 0; i < 12; i++)
    {
        in[i] = boxes[i][in[i]];
    }
}

void diffuse(uint16_t *in)
{
    uint16_t out[12];
    out[0] = in[0] ^ in[5] ^ in[6] ^ in[7] ^ in[8] ^ in[9] ^ in[10];
    out[1] = in[1] ^ in[4] ^ in[5] ^ in[6] ^ in[9] ^ in[10] ^ in[11];
    out[2] = in[2] ^ in[3] ^ in[4] ^ in[5] ^ in[6] ^ in[8] ^ in[10];
    out[3] = in[2] ^ in[3] ^ in[6] ^ in[7] ^ in[9] ^ in[10] ^ in[11];
    out[4] = in[1] ^ in[2] ^ in[4] ^ in[6] ^ in[7] ^ in[8] ^ in[9];
    out[5] = in[0] ^ in[1] ^ in[2] ^ in[5] ^ in[7] ^ in[9] ^ in[11];
    out[6] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[6] ^ in[11];
    out[7] = in[0] ^ in[3] ^ in[4] ^ in[5] ^ in[7] ^ in[10] ^ in[11];
    out[8] = in[0] ^ in[2] ^ in[4] ^ in[8] ^ in[9] ^ in[10] ^ in[11];
    out[9] = in[0] ^ in[1] ^ in[3] ^ in[4] ^ in[5] ^ in[8] ^ in[9];
    out[10] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[7] ^ in[8] ^ in[10];
    out[11] = in[1] ^ in[3] ^ in[5] ^ in[6] ^ in[7] ^ in[8] ^ in[11];
    memcpy(in, out, 24);
}

/**
 * @description: encrypt in using LSTable-(12,R) and store the result in out
 * @param {uint16_t*} in: plaintext
 * @param {uint16_t*} out: ciphertext
 * @param {int} num_r: number of rounds
 * @return {*}
 */
void encrypt(uint16_t *in, uint16_t *out, int num_r)
{
    memcpy(out, in, 24);
    for (int r = 0; r < num_r; r++)
    {
        // round function
        confuse(out, sboxes);
        diffuse(out);
    }
}

void decrypt(uint16_t *in, uint16_t *out, int num_r)
{
    memcpy(out, in, 24);
    for (int r = 0; r < num_r; r++)
    {
        // round function
        diffuse(out);
        confuse(out, inv_sboxes);
    }
}

/**
 * @description: evaluate efficiency of LSTable-(12,R)
 * @return {*}
 */
void main()
{
    struct timespec start, end;
    double time_use;
    int num_repeat_tg = 10, num_repeat_enc = 1024 * 1024;

    // test table generation
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_repeat_tg; i++)
    {
        table_gen(sboxes, inv_sboxes, 12);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_use = time_sec(start, end);
    printf("LSTable-(12,R) table generation time(s): %2f\n", time_use / num_repeat_tg);

    // test functionality
    printf("Functionality test: \n");
    uint16_t block[12], encrypted_block[12], decrypted_block[12];
    for (int i = 0; i < 12; i++)
    {
        block[i] = rand();
    }
    print_array(block, 12);
    encrypt(block, encrypted_block, 18);
    print_array(encrypted_block, 12);
    decrypt(encrypted_block, decrypted_block, 18);
    print_array(decrypted_block, 12);
    int pass = 1;
    for (int i = 0; i < 12; i++)
    {
        // printf("hhh.\n");
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

    // prepare plaintext
    size_t data_len = 12 * sizeof(uint16_t) * num_repeat_enc;
    uint16_t *plaintext = malloc(data_len);
    uint16_t *ciphertext = malloc(data_len);
    srand(time(NULL));
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    for (int r = 0; r <= 4; r++)
    {
        int num_r = r * 2 + 8;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < num_repeat_enc; i++)
        {
            encrypt(plaintext + i * 12, ciphertext + i * 12, num_r);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        int a = rand() % num_repeat_enc;
        time_use = time_sec(start, end);
        printf("LSTable-(12,%d) throught(MB/s): %2f\n", num_r, 24 * num_repeat_enc / 1024 / 1024 / time_use);
    }
}
