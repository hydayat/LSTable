/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-22 11:40:19
 * @LastEditTime: 2025-04-22 14:55:09
 * @Description:
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#include <stdio.h>
#include "ciphers/lstable.h"
#include "ciphers/pc16.h"
#include "ciphers/space16.h"
#include "test/test_framework.h"

/**
 * @description: evaluate efficiency and functionality of LSTable
 * @return {*}
 */
void test_lstable()
{
    // test configure
    int l_list[3] = {8, 12, 16};
    int min_r_list[3] = {12, 8, 6};
    int max_r_list[3] = {24, 16, 12};
    int num_repeat_tg = 10, num_repeat_enc = 1024 * 1024;

    for (int i = 0; i < 3; i++)
    {
        int l = l_list[i], min_r = min_r_list[i], max_r = max_r_list[i];
        printf("************* Test LSTable-(%d, R) *************\n", l);

        // test throughput of table generation
        init(l, max_r);
        double exec_time = test_tablegen_time(num_repeat_tg, lstable_table_gen);
        printf("Table generation time(s): %2f\n", exec_time);

        // test functionality
        printf("Functionality test for LSTable-(%d, %d): ", l, max_r);
        int result = test_func(l * 2, lstable_encrypt, lstable_decrypt);
        result == 0 ? printf("fail\n") : printf("Success\n");

        // test throughput of encryption
        for (int r = min_r; r <= max_r; r += 2)
        {
            init(l, r);
            double throughput = test_enc_throughput(l * 2, num_repeat_enc, lstable_encrypt);
            printf("LSTable-(%d,%d) throught(MB/s): %2f\n", l, r, throughput);
        }

        printf("\n");
    }
}

/**
 * @description: evaluate efficiency of PC16
 * @return {*}
 */
void test_pc16(void)
{
    printf("************* Test PC16 *************\n");

    // test configure
    int num_repeat_enc = 1024 * 1024;
    pc16_table_gen();

    // test throughput of encryption
    double throughput = test_enc_throughput(16, num_repeat_enc, pc16_encrypt);
    printf("Throught(MB/s): %2f\n", throughput);

    printf("\n");
}

/**
 * @description: evaluate efficiency of SPACE16
 * @return {*}
 */
void test_space16(void)
{
    printf("************* Test SPACE-16 *************\n");

    // test configure
    int num_repeat_enc = 1024 * 1024;
    space16_table_gen();

    // test throughput of encryption
    double throughput = test_enc_throughput(16, num_repeat_enc, space16_encrypt);
    printf("Throught(MB/s): %2f\n", throughput);

    printf("\n");
}

void main()
{
    test_pc16();
    test_space16();
    test_lstable();
}