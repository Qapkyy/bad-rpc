#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_QUANTUM 2

#ifdef _WIN32
	#define PLATFORM_NAME "Windows Ugly"
	#include <windows.h>
	#include <process.h>
#else
	#define PLATFORM_NAME "Bad Linux"
	#include <unistd.h>
	#include <sys/wait.h>
	#include <sys/time.h>
#endif

typedef struct {
	int pid;
	char task_name[50];
	int    arrival_time;
    	int    burst_time;
    	int    remaining_time;
    	double calc_start_time;
    	double calc_completion_time;
    	double calc_turnaround_time;
    	double calc_waiting_time;
    	double calc_response_time;
    	int    first_response;
} Process;

double get_wall_time(void)
{
#ifdef _WIN32
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (double)uli.QuadPart / 10000000.0;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}


