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
        cpu_intensive_work(quantum);
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

void calculate_rr_times(Process p[], int n, int quantum) {
    double current_time = 0;
    int    completed    = 0;

    for (int i = 0; i < n; i++) {
        p[i].remaining_time = p[i].burst_time;
        p[i].first_response = 1;
        p[i].calc_start_time = -1;
    }

    int queue[200], front = 0, rear = 0;
    int in_queue[200];
    for (int i = 0; i < n; i++) in_queue[i] = 0;

    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i]   = 1;
        }
    }

    while (completed < n) {
        if (front == rear) {
            double next_arrival = 999999;
            int    next_idx     = -1;
            for (int i = 0; i < n; i++) {
                if (p[i].remaining_time > 0 && p[i].arrival_time > current_time) {
                    if (p[i].arrival_time < next_arrival) {
                        next_arrival = p[i].arrival_time;
                        next_idx     = i;
                    }
                }
            }
            if (next_idx != -1) {
                current_time    = next_arrival;
                queue[rear++]   = next_idx;
                in_queue[next_idx] = 1;
            }
        } else {
            int idx = queue[front++];

            if (p[idx].first_response) {
                p[idx].calc_start_time    = current_time;
                p[idx].calc_response_time = current_time - p[idx].arrival_time;
                p[idx].first_response     = 0;
            }

            int exec_time = (p[idx].remaining_time < quantum)
                            ? p[idx].remaining_time : quantum;
            p[idx].remaining_time -= exec_time;
            current_time          += exec_time;

            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && p[i].arrival_time <= current_time
                    && p[i].remaining_time > 0) {
                    queue[rear++] = i;
                    in_queue[i]   = 1;
                }
            }

            if (p[idx].remaining_time > 0) {
                queue[rear++] = idx;
            } else {
                p[idx].calc_completion_time  = current_time;
                p[idx].calc_turnaround_time  = p[idx].calc_completion_time
                                               - p[idx].arrival_time;
                p[idx].calc_waiting_time     = p[idx].calc_turnaround_time
                                               - p[idx].burst_time;
                completed++;
                in_queue[idx] = 0;
            }
        }
    }
}


