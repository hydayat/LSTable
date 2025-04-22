/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:33:30
 * @LastEditTime: 2025-04-22 12:38:41
 * @Description: Implementation and Test Cases of LSTable-(8,R).
 *
 * Copyright (c) 2024 by Tongji University, All Rights Reserved.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>
#include <wmmintrin.h>

// parameters
int param_l, param_r;
int block_len; // in bytes

// lookup table
uint16_t (*sboxes)[1 << 16];     // for encryption
uint16_t (*inv_sboxes)[1 << 16]; // for decryption

// diffusion function
void (*diffuse)(uint16_t *);

// round keys for aes encryption
__m128i lstable_round_keys[11];

/**
 * @description: encrypt the plaintext using aes
 * @param {__m128i} in: plaintext
 * @return {__m128i} : ciphertext
 */
__m128i aes(__m128i in)
{
    in = _mm_xor_si128(in, lstable_round_keys[0]);
    in = _mm_aesenc_si128(in, lstable_round_keys[1]);
    in = _mm_aesenc_si128(in, lstable_round_keys[2]);
    in = _mm_aesenc_si128(in, lstable_round_keys[3]);
    in = _mm_aesenc_si128(in, lstable_round_keys[4]);
    in = _mm_aesenc_si128(in, lstable_round_keys[5]);
    in = _mm_aesenc_si128(in, lstable_round_keys[6]);
    in = _mm_aesenc_si128(in, lstable_round_keys[7]);
    in = _mm_aesenc_si128(in, lstable_round_keys[8]);
    in = _mm_aesenc_si128(in, lstable_round_keys[9]);
    in = _mm_aesenclast_si128(in, lstable_round_keys[10]);
    return in;
}

/**
 * @description: calculate the result of a modulo b
 * @param {__m128i} a: dividend
 * @param {int} b: modulus
 * @return {int}: result
 */
int mod(__m128i a, int b)
{
    unsigned char data[16];
    _mm_storeu_si128((__m128i *)data, a);
    int res = 0;
    for (int i = 15; i >= 0; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            int bit = (data[i] >> j) & 1;
            res = (res << 1) + bit;
            res %= b;
        }
    }
    return res;
}

/**
 * @description: generate the lookup table of LSTable
 * @return {*}
 */
void lstable_table_gen()
{
    size_t boxes_len = param_l * (1 << 16) * sizeof(uint16_t);
    sboxes = malloc(boxes_len);
    inv_sboxes = malloc(boxes_len);

    // Fisher-Yates shuffle
    for (int i = 0; i < param_l; i++)
    {
        for (int u = 0; u < (1 << 16); u++)
        {
            sboxes[i][u] = u;
        }

        for (int v = (1 << 16) - 1; v > 0; v--)
        {
            __m128i state = _mm_set_epi32(v, 0, 0, 0);
            state = aes(state);
            int k = mod(state, v);
            int temp = sboxes[i][k];
            sboxes[i][k] = sboxes[i][v];
            sboxes[i][v] = temp;
        }

        for (int w = 0; w < (1 << 16); w++)
        {
            inv_sboxes[i][sboxes[i][w]] = w;
        }
    }
}

void diffuse_8(uint16_t *in)
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

void diffuse_12(uint16_t *in)
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

void diffuse_16(uint16_t *in)
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

void confuse(uint16_t *in, uint16_t (*boxes)[1 << 16])
{
    for (int i = 0; i < param_l; i++)
    {
        in[i] = boxes[i][in[i]];
    }
}

void init(int l, int r)
{
    param_l = l;
    param_r = r;
    block_len = param_l * sizeof(uint16_t);
    switch (l)
    {
    case 8:
        diffuse = diffuse_8;
        break;
    case 12:
        diffuse = diffuse_12;
        break;
    case 16:
        diffuse = diffuse_16;
        break;
    default:
        diffuse = NULL;
        break;
    }
}

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {uint8_t*} in: plaintext
 * @param {uint8_t*} out: ciphertext
 * @return {*}
 */
void lstable_encrypt(uint8_t *in, uint8_t *out)
{
    memcpy(out, in, block_len);
    for (int r = 0; r < param_r; r++)
    {
        // round function
        confuse((uint16_t *)out, sboxes);
        diffuse((uint16_t *)out);
    }
}

/**
 * @description: decrypt in using LSTable-(8,R) and store the result in out
 * @param {uint8_t*} in: ciphertext
 * @param {uint8_t*} out: plaintext
 * @return {*}
 */
void lstable_decrypt(uint8_t *in, uint8_t *out)
{
    memcpy(out, in, block_len);
    for (int r = 0; r < param_r; r++)
    {
        // round function
        diffuse((uint16_t *)out);
        confuse((uint16_t *)out, inv_sboxes);
    }
}
