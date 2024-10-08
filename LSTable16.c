/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:45:37
 * @LastEditTime: 2024-09-30 22:44:14
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

// lookup table
uint16_t sboxes[16][1 << 16];     // for encryption
uint16_t inv_sboxes[12][1 << 16]; // for decryption

/**
 * @description: encrypt in using LSTable-(16,R) and store the result in out
 * @param {uint16_t*} in: plaintext
 * @param {uint16_t*} out: ciphertext
 * @param {int} numR: number of rounds
 * @return {*}
 */
void encrypt(uint16_t *in, uint16_t *out, int numR)
{
    uint16_t arr[16]; // state
    memcpy(out, in, 32);

    // round function
    for (int r = 0; r < numR; r++)
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
        arr[8] = sboxes[8][out[8]];
        arr[9] = sboxes[9][out[9]];
        arr[10] = sboxes[10][out[10]];
        arr[11] = sboxes[11][out[11]];
        arr[12] = sboxes[12][out[12]];
        arr[13] = sboxes[13][out[13]];
        arr[14] = sboxes[14][out[14]];
        arr[15] = sboxes[15][out[15]];

        // difussion layer
        out[0] = arr[1] ^ arr[2] ^ arr[3] ^ arr[4] ^ arr[5] ^ arr[6] ^ arr[7] ^ arr[8] ^ arr[9] ^ arr[10] ^ arr[11] ^ arr[12] ^ arr[13] ^ arr[14] ^ arr[15];
        out[1] = arr[0] ^ arr[6] ^ arr[7] ^ arr[10] ^ arr[11] ^ arr[14] ^ arr[15];
        out[2] = arr[0] ^ arr[5] ^ arr[7] ^ arr[9] ^ arr[11] ^ arr[13] ^ arr[15];
        out[3] = arr[0] ^ arr[4] ^ arr[7] ^ arr[8] ^ arr[11] ^ arr[12] ^ arr[14] ^ arr[15];
        out[4] = arr[0] ^ arr[3] ^ arr[7] ^ arr[9] ^ arr[10] ^ arr[13] ^ arr[14];
        out[5] = arr[0] ^ arr[2] ^ arr[7] ^ arr[8] ^ arr[10] ^ arr[12] ^ arr[14];
        out[6] = arr[0] ^ arr[1] ^ arr[7] ^ arr[8] ^ arr[9] ^ arr[12] ^ arr[13];
        out[7] = arr[0] ^ arr[1] ^ arr[2] ^ arr[3] ^ arr[4] ^ arr[5] ^ arr[6];
        out[8] = arr[0] ^ arr[3] ^ arr[5] ^ arr[6] ^ arr[9] ^ arr[10] ^ arr[11];
        out[9] = arr[0] ^ arr[2] ^ arr[4] ^ arr[6] ^ arr[8] ^ arr[10] ^ arr[11];
        out[10] = arr[0] ^ arr[1] ^ arr[4] ^ arr[5] ^ arr[8] ^ arr[9] ^ arr[11];
        out[11] = arr[0] ^ arr[1] ^ arr[2] ^ arr[3] ^ arr[12] ^ arr[13] ^ arr[14];
        out[12] = arr[0] ^ arr[3] ^ arr[5] ^ arr[6] ^ arr[9] ^ arr[10] ^ arr[11];
        out[13] = arr[0] ^ arr[2] ^ arr[4] ^ arr[6] ^ arr[8] ^ arr[10] ^ arr[11];
        out[14] = arr[0] ^ arr[1] ^ arr[4] ^ arr[5] ^ arr[8] ^ arr[9] ^ arr[11];
        out[15] = arr[0] ^ arr[1] ^ arr[2] ^ arr[3] ^ arr[8] ^ arr[9] ^ arr[10];
    }
}

/**
 * @description: evaluate efficiency of LSTable-(16,R)
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
        table_gen(sboxes, inv_sboxes, 16);
    }
    c_end = clock();
    time_use = (double)(c_end - c_start) / CLOCKS_PER_SEC;
    printf("LSTable-(16,R) table generation time(s): %2f\n", time_use / num_repeat_tg);

    // test encryption
    uint16_t *plaintext = malloc(16 * sizeof(uint16_t) * num_repeat_enc);
    uint16_t *ciphertext = malloc(16 * sizeof(uint16_t) * num_repeat_enc);
    for (int r = 0; r <= 3; r++)
    {
        int numR = r * 2 + 6;
        clock_t c_start = clock();
        for (int i = 0; i < num_repeat_enc; i++)
        {
            encrypt(plaintext + i * 16, ciphertext + i * 16, numR);
        }
        clock_t c_end = clock();
        int a = rand() % num_repeat_enc;
        double timeuse = (double)(c_end - c_start) / CLOCKS_PER_SEC;
        printf("LSTable-(16,%d) throught(MB/s): %2f\n", numR, 32 * num_repeat_enc / 1024 / 1024 / timeuse);
    }
}
