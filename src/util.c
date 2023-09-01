#include "include/util.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long current_time_microseconds()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);

    unsigned long time_nanoseconds = time.tv_sec * 1000000000 + time.tv_nsec;
    return time_nanoseconds / 1000;
}

void start_timer()
{
    time_ms_at_timer_start = current_time_microseconds();
}

void stop_timer(char* msg)
{
    unsigned long time_ms = current_time_microseconds();
    unsigned long time_elapsed = time_ms - time_ms_at_timer_start;

    printf("\n%s: %lu microseconds elapsed.\n\n", msg, time_elapsed);
}

char* load_file(char* name, unsigned long* size)
{
    FILE* file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = malloc(*size + 1);
    fread(data, 1, *size, file);
    data[*size] = 0;
    fclose(file);

    return data;
}