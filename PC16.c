/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 22:23:32
 * @LastEditTime: 2025-04-21 16:46:28
 * @Description: Implementation and Test Cases of PC16.
 *
 * Copyright (c) 2024 by Tongji University, All Rights Reserved.
 */
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include <wmmintrin.h>
#include "aes_software/aes.h"
#include "TestFramework.h"

// round keys for aes encryption
__m128i round_keys[11];

/**
 * @description: encrypt the plaintext using aes_ni
 * @param {__m128i} in: plaintext
 * @return {__m128i} : ciphertext
 */
__m128i aes_ni(__m128i in)
{
    in = _mm_xor_si128(in, round_keys[0]);
    in = _mm_aesenc_si128(in, round_keys[1]);
    in = _mm_aesenc_si128(in, round_keys[2]);
    in = _mm_aesenc_si128(in, round_keys[3]);
    in = _mm_aesenc_si128(in, round_keys[4]);
    in = _mm_aesenc_si128(in, round_keys[5]);
    in = _mm_aesenc_si128(in, round_keys[6]);
    in = _mm_aesenc_si128(in, round_keys[7]);
    in = _mm_aesenc_si128(in, round_keys[8]);
    in = _mm_aesenc_si128(in, round_keys[9]);
    in = _mm_aesenclast_si128(in, round_keys[10]);
    return in;
}

// lookup table
unsigned char T[4][1 << 16][8];

/**
 * @description: generate the lookup table of PC16
 * @return {*}
 */
void gen()
{
    srand(time(NULL));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < (1 << 16); j++)
            for (int k = 0; k < 8; k++)
                T[i][j][k] = rand() % 256;
    // Note that encryption efficiencies when using the randomly generated table and real table are nearly the same.
}

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {unsigned int*} in: plaintext
 * @param {unsigned int*} out: ciphertext
 * @return {*}
 */
void encrypt(unsigned char *in,
             unsigned char *out)
{
    unsigned int t0, t1, t2, t3;
    unsigned char text[16];
    unsigned char temp[16];
    int i, r;

    t0 = (in[0] << 8) | (in[1] & 0xff);
    t1 = (in[2] << 8) | (in[3] & 0xff);
    t2 = (in[4] << 8) | (in[5] & 0xff);
    t3 = (in[6] << 8) | (in[7] & 0xff);

    for (i = 0; i < 8; i++)
    {
        text[8 + i] = in[8 + i] ^ T[0][t0][i] ^ T[1][t1][i] ^ T[2][t2][i] ^ T[3][t3][i];
    }

    memcpy(text, in, 8);

    for (r = 0; r < 18; ++r)
    {
        // encrypt using aes-ni
        // __m128i res = aes_ni(*((__m128i*) text));
        // memcpy(&res, temp, 16);

        // encrypt using software-based aes
        aes_cipher(text, temp, (uint8_t *)round_keys);

        t0 = (temp[0] << 8) | (temp[1] & 0xff);
        t1 = (temp[2] << 8) | (temp[3] & 0xff);
        t2 = (temp[4] << 8) | (temp[5] & 0xff);
        t3 = (temp[6] << 8) | (temp[7] & 0xff);

        for (i = 0; i < 8; i++)
        {
            text[8 + i] = in[8 + i] ^ T[0][t0][i] ^ T[1][t1][i] ^ T[2][t2][i] ^ T[3][t3][i];
        }

        memcpy(text, temp, 8);
    }
    memcpy(out, text, 16);
}

/**
 * @description: evaluate efficiency of PC16
 * @return {*}
 */
void main(void)
{
    // table generation
    gen();

    // randomly init aes key
    for (int i = 0; i <= 10; i++)
    {
        round_keys[i] = _mm_set_epi32(rand(), rand(), rand(), rand());
    }

    // prepare plaintext
    struct timespec start, end;
    int num_repeat = 1024 * 1024;
    size_t data_len = 16 * sizeof(unsigned char) * num_repeat;
    unsigned char *plaintext = malloc(data_len);
    unsigned char *ciphertext = malloc(data_len);
    srand(time(NULL));
    for (int i = 0; i < data_len / sizeof(int); i++)
    {
        ((int *)plaintext)[i] = rand();
    }

    // test encryption
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < num_repeat; i++)
    {
        encrypt(plaintext + i * 16, ciphertext + i * 16);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_use = time_sec(start, end);
    printf("PC16 throught(MB/s): %2f\n", 16 * num_repeat / 1024 / 1024 / time_use);
}
