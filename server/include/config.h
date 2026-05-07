/*
 * config.h
 * This header defines the Config structure and function prototypes for managing alert thresholds for CPU usage, memory usage, CPU temperature, GPU temperature, and storage usage.
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct
{

    double cpu_usage_limit;
    double mem_usage_limit;
    double cpu_temp_limit;
    double gpu_temp_limit;
    double storage_usage_limit;
} Config;

extern Config config;

// Function prototypes to set default configuration and update alert limits
void set_default_config();

void set_cpu_alert_limit(double limit);
void set_mem_alert_limit(double limit);
void set_cpu_temp_alert_limit(double limit);
void set_gpu_temp_alert_limit(double limit);
void set_storage_alert_limit(double limit);
#endif