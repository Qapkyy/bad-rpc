#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_QUANTUM 2

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

void cpu_intensive_work(int seconds)
{
    double target = get_wall_time() + seconds;
    volatile double result = 0.0;
    while (get_wall_time() < target) {
        for (volatile long i = 0; i < 10000000L; i++) {
            result += i * 3.14159;
            result  = result / 2.71828;
        }
    }
}

void execute_process_quantum(Process *p, int quantum) {
#ifdef _WIN32
    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    char exe_path[MAX_PATH];
    char cmdline[MAX_PATH + 32];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    snprintf(cmdline, sizeof(cmdline), "\"%s\" -work %d", exe_path, quantum);

    if (CreateProcessA(NULL, cmdline, NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        cpu_intensive_work(quantum);   /* fallback */
    }
#else
    pid_t child = fork();
    if (child == 0) {
        cpu_intensive_work(quantum);
        exit(0);
    } else {
        int status;
        waitpid(child, &status, 0);
    }
#endif
}

void calculate_rr_times()
{

}
