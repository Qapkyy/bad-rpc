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








void execute_process(Process *p)
{
#ifdef _WIN32
    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    char cmdline[128];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    snprintf(cmdline, sizeof(cmdline), "\"%s\" -work %d",
             "", p->burst_time);

    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    snprintf(cmdline, sizeof(cmdline), "\"%s\" -work %d",
             exe_path, p->burst_time);

    if (CreateProcessA(
            NULL,
            cmdline,
            NULL, NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL, NULL,
            &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        cpu_intensive_work(p->burst_time);
    }

#else
    pid_t child = fork();
    if (child == 0) {
        cpu_intensive_work(p->burst_time);
        exit(0);
    } else {
        int status;
        waitpid(child, &status, 0);
    }
#endif
}

void portable_sleep(int seconds)
{
#ifdef _WIN32
    Sleep((DWORD)(seconds * 1000));
#else
    sleep((unsigned int)seconds);
#endif
}

void calculate_fcfs_times(Process p[], int n)
{
	double current_time = 0;
    	for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival_time)
            current_time = p[i].arrival_time;

        p[i].calc_start_time       = current_time;
        p[i].calc_completion_time  = current_time + p[i].burst_time;
        p[i].calc_turnaround_time  = p[i].calc_completion_time - p[i].arrival_time;
        p[i].calc_waiting_time     = p[i].calc_start_time - p[i].arrival_time;
        p[i].calc_response_time    = p[i].calc_start_time - p[i].arrival_time;

        current_time = p[i].calc_completion_time;
    }
}

void print_table()
{

}

int main()
{


	return 0;
}
