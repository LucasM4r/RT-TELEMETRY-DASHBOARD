/*
 * main.c
 * Entry point for the system monitoring server application.
 * Initializes system information, starts monitoring threads, and runs the network server.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "../include/monitor.h"
#include "../include/network.h"
#include "../include/info.h"
#include "../include/config.h"

// Global variables
SystemData data;
Config config;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alert_cond = PTHREAD_COND_INITIALIZER;
SystemInfo static_info;

void handle_sig(int sig)
{
    pthread_mutex_lock(&data_mutex);
    data.running = 0;
    pthread_cond_broadcast(&alert_cond);
    pthread_mutex_unlock(&data_mutex);
}

int main()
{
    // Create threads for monitoring and network server
    pthread_t t_cpu, t_mem, t_gpu, t_alert, t_network, t_network_monitor, t_storage_monitor;

    // Initialize shared data
    data.running = 1;
    data.cpu_usage = 0.0;
    data.mem_usage = 0.0;
    data.gpu_vram_usage = 0.0;
    data.net_down = 0.0;
    data.storage_temp = 0.0;
    data.alert_active = 0;
    data.alert_enabled = 1;
    data.latency = 0;
    data.mem_used = 0.0;
    data.mem_cached = 0.0;
    data.gpu_temp = 0.0;
    data.gpu_vram_used = 0.0;
    data.net_up = 0.0;
    data.net_total_down = 0.0;
    data.net_total_up = 0.0;
    data.storage_used = 0.0;
    data.storage_usage = 0.0;
    data.storage_read = 0.0;
    data.storage_write = 0.0;
    data.storage_temp = 0.0;
    data.cpu_freq = 0.0;

    set_default_config();
    for (int i = 0; i < MAX_CORES; i++)
    {
        data.cpu_cores[i] = 0.0;
        data.last_idle_cores[i] = 0;
        data.last_total_cores[i] = 0;
    }

    static_info = init_system_info();

    printf("CPU Model: %s\n", static_info.cpu_model);
    printf("CPU Cores: %d\n", static_info.cpu_cores);
    printf("CPU Threads: %d\n", static_info.cpu_threads);
    printf("Total RAM: %.2f GB\n", static_info.mem_total);
    printf("GPU Model: %s\n", static_info.gpu_model);
    printf("GPU VRAM: %.2f GB\n", static_info.gpu_mem_total);

    signal(SIGINT, handle_sig);

    printf("Initializing...\n");
    printf("Press Ctrl+C to stop.\n\n");

    // Initialize POSIX timers
    if (init_timers() == -1)
    {
        fprintf(stderr, "Failed to initialize timers\n");
        return 1;
    }

    // Create Threads
    if (pthread_create(&t_cpu, NULL, thread_cpu_monitor, NULL) != 0)
    {
        perror("Failed to create CPU monitor thread");
        return 1;
    }

    if (pthread_create(&t_mem, NULL, thread_memory_monitor, NULL) != 0)
    {
        perror("Failed to create Memory monitor thread");
        return 1;
    }

    if (pthread_create(&t_gpu, NULL, thread_gpu_monitor, NULL) != 0)
    {
        perror("Failed to create GPU monitor thread");
        return 1;
    }

    if (pthread_create(&t_network_monitor, NULL, thread_network_monitor, NULL) != 0)
    {
        perror("Failed to create Network monitor thread");
        return 1;
    }

    if (pthread_create(&t_storage_monitor, NULL, thread_storage_monitor, NULL) != 0)
    {
        perror("Failed to create Storage monitor thread");
        return 1;
    }

    if (pthread_create(&t_alert, NULL, thread_alert_handler, NULL) != 0)
    {
        perror("Failed to create Alert handler thread");
        return 1;
    }

    if (pthread_create(&t_network, NULL, thread_network, NULL) != 0)
    {
        perror("Failed to create Network thread");
        return 1;
    }

    while (data.running)
    {
        pthread_mutex_lock(&data_mutex);
        printf("\r[MONITOR] CPU: %6.2f%% | RAM: %6.2f%% (%.2f GB livre) | GPU: %6.2f%% (%.2f GB livre) | Network: %6.2f Mbps down | Storage: %6.2f°C",
               data.cpu_usage, data.mem_usage, data.mem_used, data.gpu_vram_usage, data.gpu_vram_used, data.net_down, data.storage_temp);

        fflush(stdout);
        pthread_mutex_unlock(&data_mutex);

        usleep(500000);
    }

    // Signal alert handler to exit if waiting
    pthread_cond_signal(&alert_cond);

    // Wait for threads to finish
    pthread_join(t_cpu, NULL);
    pthread_join(t_mem, NULL);
    pthread_join(t_gpu, NULL);
    pthread_join(t_network, NULL);
    pthread_join(t_alert, NULL);
    pthread_join(t_network_monitor, NULL);
    pthread_join(t_storage_monitor, NULL);

    // Destroy mutex and condition variable
    pthread_mutex_destroy(&config_mutex);
    pthread_mutex_destroy(&data_mutex);
    pthread_cond_destroy(&alert_cond);

    // Cleanup POSIX timers
    cleanup_timers();

    printf("\nTest completed.\n");

    return 0;
}