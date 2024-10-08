/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 15:30:55
 * @LastEditTime: 2024-10-02 21:17:52
 * @Description: Implementation of TableGen
 *
 * Copyright (c) 2024 by Tongji University, All Rights Reserved.
 */
#include <x86intrin.h>
#include <wmmintrin.h>
#include <stdint.h>
#include <stdlib.h>

// round keys for aes encryption
__m128i round_keys[11];

/**
 * @description: encrypt the plaintext using aes
 * @param {__m128i} in: plaintext
 * @return {__m128i} : ciphertext
 */
__m128i aes(__m128i in)
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
 * @description: generate the lookup table of TableGen
 * @param {uint16[1<<16]*} sboxes: lookup table for encryption
 * @param {uint16[1<<16]*} inv_sboxes: lookup table for decryption
 * @param {int} sizeL: size of MDS matrix
 * @return {*}
 */
void table_gen(uint16_t (*sboxes)[1 << 16], uint16_t (*inv_sboxes)[1 << 16], int sizeL)
{
  // Fisher-Yates shuffle
  for (int i = 0; i < sizeL; i++)
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
