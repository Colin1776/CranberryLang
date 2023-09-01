#ifndef UTIL_H
#define UTIL_H

static unsigned long time_ms_at_timer_start = 0;

unsigned long current_time_microseconds();
void start_timer();
void stop_timer(char*);
char* load_file(char*, unsigned long*);


#endif
