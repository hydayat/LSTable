/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:45:37
 * @LastEditTime: 2025-04-21 22:04:03
 * @Description: Implementation and Test Cases of LSTable-(16,R).
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
uint16_t sboxes[16][1 << 16];     // for encryption
uint16_t inv_sboxes[16][1 << 16]; // for decryption

void confuse(uint16_t *in, uint16_t boxes[16][1 << 16])
{
    for (int i = 0; i < 16; i++)
    {
        in[i] = boxes[i][in[i]];
    }
}

void diffuse(uint16_t *in)
{
    uint16_t out[16];
    out[0] = in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[5] ^ in[6] ^ in[7] ^ in[8] ^ in[9] ^ in[10] ^ in[11] ^ in[12] ^ in[13] ^ in[14] ^ in[15];
    out[1] = in[0] ^ in[6] ^ in[7] ^ in[10] ^ in[11] ^ in[14] ^ in[15];
    out[2] = in[0] ^ in[5] ^ in[7] ^ in[9] ^ in[11] ^ in[13] ^ in[15];
    out[3] = in[0] ^ in[4] ^ in[7] ^ in[8] ^ in[11] ^ in[12] ^ in[15];
    out[4] = in[0] ^ in[3] ^ in[7] ^ in[9] ^ in[10] ^ in[13] ^ in[14];
    out[5] = in[0] ^ in[2] ^ in[7] ^ in[8] ^ in[10] ^ in[12] ^ in[14];
    out[6] = in[0] ^ in[1] ^ in[7] ^ in[8] ^ in[9] ^ in[12] ^ in[13];
    out[7] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[5] ^ in[6];
    out[8] = in[0] ^ in[3] ^ in[5] ^ in[6] ^ in[13] ^ in[14] ^ in[15];
    out[9] = in[0] ^ in[2] ^ in[4] ^ in[6] ^ in[12] ^ in[14] ^ in[15];
    out[10] = in[0] ^ in[1] ^ in[4] ^ in[5] ^ in[12] ^ in[13] ^ in[15];
    out[11] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[12] ^ in[13] ^ in[14];
    out[12] = in[0] ^ in[3] ^ in[5] ^ in[6] ^ in[9] ^ in[10] ^ in[11];
    out[13] = in[0] ^ in[2] ^ in[4] ^ in[6] ^ in[8] ^ in[10] ^ in[11];
    out[14] = in[0] ^ in[1] ^ in[4] ^ in[5] ^ in[8] ^ in[9] ^ in[11];
    out[15] = in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[8] ^ in[9] ^ in[10];
    memcpy(in, out, 32);
}

/**
 * @description: encrypt in using LSTable-(16,R) and store the result in out
 * @param {uint16_t*} in: plaintext
 * @param {uint16_t*} out: ciphertext
 * @param {int} numR: number of rounds
 * @return {*}
 */
void encrypt(uint16_t *in, uint16_t *out, int numR)
{
    memcpy(out, in, 32);
    for (int r = 0; r < numR; r++)
    {
        // round function
        confuse(out, sboxes);
        diffuse(out);
    }
}

void decrypt(uint16_t *in, uint16_t *out, int numR)
{
    memcpy(out, in, 32);
    for (int r = 0; r < numR; r++)
    {
        // round function
        diffuse(out);
        confuse(out, inv_sboxes);
    }
}

/**
 * @description: evaluate efficiency of LSTable-(16,R)
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
        table_gen(sboxes, inv_sboxes, 16);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_use = time_sec(start, end);
    printf("LSTable-(16,R) table generation time(s): %2f\n", time_use / num_repeat_tg);

    // test functionality
    printf("Functionality test: \n");
    uint16_t block[16], encrypted_block[16], decrypted_block[16];
    for (int i = 0; i < 16; i++)
    {
        block[i] = rand();
    }
    print_array(block, 16);
    encrypt(block, encrypted_block, 12);
    print_array(encrypted_block, 16);
    decrypt(encrypted_block, decrypted_block, 12);
    print_array(decrypted_block, 16);
    int pass = 1;
    for (int i = 0; i < 16; i++)
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
    size_t data_len = 16 * sizeof(uint16_t) * num_repeat_enc;
    uint16_t *plaintext = malloc(data_len);
    uint16_t *ciphertext = malloc(data_len);
    srand(time(NULL));
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    for (int r = 0; r <= 3; r++)
    {
        int numR = r * 2 + 6;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < num_repeat_enc; i++)
        {
            encrypt(plaintext + i * 16, ciphertext + i * 16, numR);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        int a = rand() % num_repeat_enc;
        double timeuse = time_sec(start, end);
        printf("LSTable-(16,%d) throught(MB/s): %2f\n", numR, 32 * num_repeat_enc / 1024 / 1024 / timeuse);
    }
}
