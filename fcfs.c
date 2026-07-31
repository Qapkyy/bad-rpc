#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define PLATFORM_NAME "Windows"
    #include <windows.h>
    #include <process.h>
#else
    #define PLATFORM_NAME "Linux"
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/time.h>
#endif

typedef struct {
    int    pid;
    char   task_name[50];
    int    arrival_time;
    int    burst_time;
    double calc_start_time;
    double calc_completion_time;
    double calc_turnaround_time;
    double calc_waiting_time;
    double calc_response_time;
} Process;

double get_wall_time(void) {
#ifdef _WIN32
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    /* 100-nanosecond intervals since 1601 → convert to seconds */
    return (double)uli.QuadPart / 10000000.0;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}

void cpu_intensive_work(int seconds) {
    double target = get_wall_time() + seconds;
    volatile double result = 0.0;
    while (get_wall_time() < target) {
        for (volatile long i = 0; i < 10000000L; i++) {
            result += i * 3.14159;
            result  = result / 2.71828;
        }
    }
}








void execute_process()
{

}

void portable_sleep()
{

}

void calculate_fcfs_times()
{

}

int main()
{


	return 0;
}
