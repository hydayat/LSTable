#include <time.h>
#include <stdint.h>

/**
 * @description: calculate the time elapsed
 * @param {struct timespec} start: start point
 * @param {struct timespec} end: end point
 * @return {double}: time elapsed (in seconds)
 */
double time_sec(struct timespec start, struct timespec end)
{
    return difftime(end.tv_sec, start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

void print_array(uint16_t *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}