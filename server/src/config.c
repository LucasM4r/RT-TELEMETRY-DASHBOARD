/*
 * config.c
 * This file implements the configuration management for alert thresholds in the system monitoring server.
 */
#include "../include/config.h"

void set_default_config()
{
    config.cpu_usage_limit = 90.0;
    config.mem_usage_limit = 95.0;
    config.cpu_temp_limit = 85.0;
    config.gpu_temp_limit = 85.0;
    config.storage_usage_limit = 98.0;
}

void set_cpu_alert_limit(double limit)
{
    config.cpu_usage_limit = limit;
}

void set_mem_alert_limit(double limit)
{
    config.mem_usage_limit = limit;
}

void set_cpu_temp_alert_limit(double limit)
{
    config.cpu_temp_limit = limit;
}

void set_gpu_temp_alert_limit(double limit)
{
    config.gpu_temp_limit = limit;
}

void set_storage_alert_limit(double limit)
{
    config.storage_usage_limit = limit;
}