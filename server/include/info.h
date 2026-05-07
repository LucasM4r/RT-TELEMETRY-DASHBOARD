/*
 * info.h
 * This header defines the SystemInfo structure and function prototypes for gathering system static information such as CPU, memory, GPU, and storage details.
 */

#ifndef INFO_H
#define INFO_H

// System information structure and functions
typedef struct
{

    char cpu_model[64];
    int cpu_cores;
    int cpu_threads;
    double mem_total;
    char gpu_model[64];
    double gpu_mem_total;
    double storage_total;
    char storage_model[64];

} SystemInfo;

extern SystemInfo static_info;

// Function prototypes to gather system information
void get_cpu_info(SystemInfo *info);
void get_mem_info(SystemInfo *info);
void get_gpu_info(SystemInfo *info);
void get_storage_info(SystemInfo *info);

SystemInfo init_system_info();

#endif