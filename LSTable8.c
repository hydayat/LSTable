/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:33:30
 * @LastEditTime: 2025-04-21 17:35:29
 * @Description: Implementation and Test Cases of LSTable-(8,R).
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
uint16_t sboxes[8][1 << 16];     // for encryption
uint16_t inv_sboxes[8][1 << 16]; // for decryption

void confuse(uint16_t *in, uint16_t boxes[8][1 << 16])
{
    for (int i = 0; i < 8; i++)
    {
        in[i] = boxes[i][in[i]];
    }
}

void diffuse(uint16_t *in)
{
    uint16_t out[8];
    out[0] = in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[7];
    out[1] = in[0] ^ in[1] ^ in[4] ^ in[5] ^ in[7];
    out[2] = in[0] ^ in[1] ^ in[2] ^ in[5] ^ in[6];
    out[3] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[6] ^ in[7];
    out[4] = in[2] ^ in[3] ^ in[5] ^ in[6] ^ in[7];
    out[5] = in[0] ^ in[2] ^ in[4] ^ in[5];
    out[6] = in[0] ^ in[1] ^ in[3] ^ in[4] ^ in[5] ^ in[6];
    out[7] = in[1] ^ in[2] ^ in[4] ^ in[5] ^ in[6] ^ in[7];
    memcpy(in, out, 16);
}

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {uint16_t*} in: plaintext
 * @param {uint16_t*} out: ciphertext
 * @param {int} num_r: number of rounds
 * @return {*}
 */
void encrypt(uint16_t *in, uint16_t *out, int num_r)
{
    memcpy(out, in, 16);
    for (int r = 0; r < num_r; r++)
    {
        // round function
        confuse(out, sboxes);
        diffuse(out);
    }
}

/**
 * @description: decrypt in using LSTable-(8,R) and store the result in out
 * @param {uint16_t*} in: ciphertext
 * @param {uint16_t*} out: plaintext
 * @param {int} num_r: number of rounds
 * @return {*}
 */
void decrypt(uint16_t *in, uint16_t *out, int num_r)
{
    memcpy(out, in, 16);
    for (int r = 0; r < num_r; r++)
    {
        // round function
        diffuse(out);
        confuse(out, inv_sboxes);
    }
}

/**
 * @description: evaluate efficiency of LSTable-(8,R)
 * @return {*}
 */
void main()
{
    struct timespec start, end;
    double time_use;
    int num_repeat_tg = 10, num_repeat_enc = 1024 * 1024;
    srand(time(NULL));

    // test table generation
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_repeat_tg; i++)
    {
        table_gen(sboxes, inv_sboxes, 8);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_use = time_sec(start, end);
    printf("LSTable-(8,R) table generation time(s): %2f\n", time_use / num_repeat_tg);

    // test functionality
    printf("Functionality test: \n");
    uint16_t block[8], encrypted_block[8], decrypted_block[8];
    for (int i = 0; i < 8; i++)
    {
        block[i] = rand();
    }
    print_array(block, 8);
    encrypt(block, encrypted_block, 24);
    print_array(encrypted_block, 8);
    decrypt(encrypted_block, decrypted_block, 24);
    print_array(decrypted_block, 8);
    int pass = 1;
    for (int i = 0; i < 8; i++)
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

    // prepare plaintext
    size_t data_len = 8 * sizeof(uint16_t) * num_repeat_enc;
    uint16_t *plaintext = malloc(data_len);
    uint16_t *ciphertext = malloc(data_len);
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    for (int r = 0; r <= 6; r++)
    {
        int num_r = r * 2 + 12;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < num_repeat_enc; i++)
        {
            encrypt(plaintext + i * 8, ciphertext + i * 8, num_r);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        int a = rand() % num_repeat_enc;
        time_use = time_sec(start, end);
        printf("LSTable-(8,%d) throught(MB/s): %2f\n", num_r, 16 * num_repeat_enc / 1024 / 1024 / time_use);
    }
}