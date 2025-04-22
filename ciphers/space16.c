/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 22:23:22
 * @LastEditTime: 2025-04-22 13:46:10
 * @Description: Implementation and Test Cases of SPACE8.
 *
 * Copyright (c) 2024 by Tongji University, All Rights Reserved.
 */
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include "space16.h"

// lookup table
unsigned int F[1 << 16][7];

/**
 * @description: generate the lookup table of SPACE16
 * @return {*}
 */
void space16_table_gen()
{
    srand(time(NULL));
    for (int i = 0; i < (1 << 16); i++)
        for (int j = 0; j < 7; j++)
            F[i][j] = rand() % (1 << 15);
    // Note that encryption efficiencies when using the randomly generated table and real table are nearly the same.
}

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {unsigned int*} in: plaintext
 * @param {unsigned int*} out: ciphertext
 * @return {*}
 */
void space16_encrypt(uint8_t *in, uint8_t *out)
{
    __m128i state = _mm_loadu_si128((__m128i *)in);
    __m128i temp;
    uint16_t t0;

    for (int r = 0; r < 128; r++)
    {
        t0 = _mm_extract_epi16(state, 0);
        temp = _mm_loadu_si128((__m128i *)F[t0]);
        state = _mm_slli_si128(state, 2); // shift left
        state = _mm_xor_si128(state, temp);
        state = _mm_insert_epi16(state, t0, 7);
    }
    _mm_store_si128((void *)out, state);
}
