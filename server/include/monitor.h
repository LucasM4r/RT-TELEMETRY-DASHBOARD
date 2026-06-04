/*
 * monitor.h
 * This header defines the SystemData structure and function prototypes for monitoring system metrics such as CPU usage, memory usage, GPU metrics, network speeds, and storage metrics. It also declares the thread functions for continuous monitoring.
 */
#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>

#define MAX_CORES 64
#define MAX_THERMAL_ZONES 10

// System data structure
typedef struct
{
    double cpu_usage;
    float cpu_freq;
    float cpu_temp;
    double cpu_cores[MAX_CORES];
    int num_cores;
    double mem_used;
    double mem_cached;
    double mem_usage;
    double gpu_temp;
    double gpu_vram_used;
    double gpu_vram_usage;
    double net_down;
    double net_up;
    double net_total_down;
    double net_total_up;
    double storage_read;
    double storage_write;
    double storage_temp;
    double storage_used;
    double storage_usage;
    int latency;
    int alert_active;
    int alert_enabled;
    int running;

    long last_idle_cores[MAX_CORES];
    long last_total_cores[MAX_CORES];
} SystemData;

// Global variables
extern SystemData data;
extern pthread_mutex_t data_mutex;
extern pthread_cond_t alert_cond;

// Helper function to locate the storage temperature sensor path
void get_storage_temp_path(char *path, size_t size);

// Function prototypes
void get_cpu_times(long *idle, long *total);
void get_cpu_freq(SystemData *data);
void get_cpu_temp(SystemData *data);

void get_cores_usage(SystemData *data);
void read_cpu_percent(SystemData *data);
void *thread_cpu_monitor(void *arg);

void read_memory_usage(SystemData *data);
void *thread_memory_monitor(void *arg);

void *thread_alert_handler(void *arg);

void get_gpu_metrics(SystemData *data);
void *thread_gpu_monitor(void *arg);

void get_network_metrics(SystemData *data);
void *thread_network_monitor(void *arg);

void get_storage_io(SystemData *data);
void get_storage_usage(SystemData *data);
void get_storage_temp(SystemData *data);
void get_storage_metrics(SystemData *data);

void *thread_storage_monitor(void *arg);

#endif