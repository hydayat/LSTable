/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-22 11:04:02
 * @LastEditTime: 2025-04-22 14:47:35
 * @Description:
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#pragma once
#include <stdint.h>

/**
 * @brief initialize LSTable's parameters
 *
 * @param l : length of LBox
 * @param r : round number
 */
void init(int l, int r);

/**
 * @description: generate the lookup table of LSTable
 * @return {*}
 */
void lstable_table_gen();

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {uint8_t*} in: plaintext
 * @param {uint8_t*} out: ciphertext
 * @return {*}
 */
void lstable_encrypt(uint8_t *in, uint8_t *out);

/**
 * @description: decrypt in using LSTable-(8,R) and store the result in out
 * @param {uint8_t*} in: ciphertext
 * @param {uint8_t*} out: plaintext
 * @return {*}
 */
void lstable_decrypt(uint8_t *in, uint8_t *out);
