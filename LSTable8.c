/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2024-09-30 11:33:30
 * @LastEditTime: 2025-04-21 23:47:47
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
 * @description: evaluate efficiency and functionality of LSTable-(8,R)
 * @return {*}
 */
void main()
{
    int num_repeat_tg = 10, num_repeat_enc = 1024 * 1024;

    // test throughput of table generation
    test_lstable_tablegen_throughput(sboxes, inv_sboxes, 8, num_repeat_tg);

    // test functionality
    test_lstable_func(8, 24, encrypt, decrypt);

    // test throughput of encryption
    test_lstable_enc_throughput(8, 12, 24, num_repeat_enc, encrypt);
}