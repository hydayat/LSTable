/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-22 13:41:31
 * @LastEditTime: 2025-04-22 14:27:13
 * @Description:
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#pragma once
#include <stdint.h>

/**
 * @description: generate the lookup table of SPACE16
 * @return {*}
 */
void space16_table_gen();

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {unsigned int*} in: plaintext
 * @param {unsigned int*} out: ciphertext
 * @return {*}
 */
void space16_encrypt(uint8_t *in, uint8_t *out);