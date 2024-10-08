/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:33:30
 * @LastEditTime: 2024-09-30 22:44:00
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

// lookup table
uint16_t sboxes[8][1 << 16];     // for encryption
uint16_t inv_sboxes[8][1 << 16]; // for decryption

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {uint16_t*} in: plaintext
 * @param {uint16_t*} out: ciphertext
 * @param {int} num_r: number of rounds
 * @return {*}
 */
void encrypt(uint16_t *in, uint16_t *out, int num_r)
{
    uint16_t arr[8]; // state
    memcpy(out, in, 16);
    // round function
    for (int r = 0; r < num_r; r++)
    {
        // confusion layer
        arr[0] = sboxes[0][out[0]];
        arr[1] = sboxes[1][out[1]];
        arr[2] = sboxes[2][out[2]];
        arr[3] = sboxes[3][out[3]];
        arr[4] = sboxes[4][out[4]];
        arr[5] = sboxes[5][out[5]];
        arr[6] = sboxes[6][out[6]];
        arr[7] = sboxes[7][out[7]];

        // diffusion layer
        out[0] = arr[1] ^ arr[2] ^ arr[3] ^ arr[4] ^ arr[7];
        out[1] = arr[0] ^ arr[1] ^ arr[4] ^ arr[5] ^ arr[7];
        out[2] = arr[0] ^ arr[1] ^ arr[2] ^ arr[5] ^ arr[6];
        out[3] = arr[0] ^ arr[1] ^ arr[2] ^ arr[3] ^ arr[6] ^ arr[7];
        out[4] = arr[2] ^ arr[3] ^ arr[5] ^ arr[6] ^ arr[7];
        out[5] = arr[0] ^ arr[2] ^ arr[4] ^ arr[5];
        out[6] = arr[0] ^ arr[1] ^ arr[3] ^ arr[4] ^ arr[5] ^ arr[6];
        out[7] = arr[1] ^ arr[2] ^ arr[4] ^ arr[5] ^ arr[6] ^ arr[7];
    }
}

/**
 * @description: evaluate efficiency of LSTable-(8,R)
 * @return {*}
 */
void main()
{
    clock_t c_start, c_end;
    double time_use;
    int num_repeat_tg = 10, num_repeat_enc = 1024 * 1024;

    // test table generation
    c_start = clock();
    for (int i = 0; i < num_repeat_tg; i++)
    {
        table_gen(sboxes, inv_sboxes, 8);
    }
    c_end = clock();
    time_use = (double)(c_end - c_start) / CLOCKS_PER_SEC;
    printf("LSTable-(8,R) table generation time(s): %2f\n", time_use / num_repeat_tg);

    // test encryption
    uint16_t *plaintext = malloc(8 * sizeof(uint16_t) * num_repeat_enc);
    uint16_t *ciphertext = malloc(8 * sizeof(uint16_t) * num_repeat_enc);
    for (int r = 0; r <= 6; r++)
    {
        int num_r = r * 2 + 12;
        c_start = clock();
        for (int i = 0; i < num_repeat_enc; i++)
        {
            encrypt(plaintext + i * 8, ciphertext + i * 8, num_r);
        }
        c_end = clock();
        int a = rand() % num_repeat_enc;
        time_use = (double)(c_end - c_start) / CLOCKS_PER_SEC;
        printf("LSTable-(8,%d) throught(MB/s): %2f\n", num_r, 16 * num_repeat_enc / 1024 / 1024 / time_use);
    }
}