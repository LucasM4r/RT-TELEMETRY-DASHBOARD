/*
 * config.c
 * This file implements the configuration management for alert thresholds in the system monitoring server.
 */
#include "../include/config.h"

static int is_valid_limit(double limit, double min, double max)
{
    return limit >= min && limit <= max;
}

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
    if (!is_valid_limit(limit, 0.0, 100.0))
        return;

    config.cpu_usage_limit = limit;
}

void set_mem_alert_limit(double limit)
{
    if (!is_valid_limit(limit, 0.0, 100.0))
        return;

    config.mem_usage_limit = limit;
}

void set_cpu_temp_alert_limit(double limit)
{
    if (!is_valid_limit(limit, 0.0, 150.0))
        return;

    config.cpu_temp_limit = limit;
}

void set_gpu_temp_alert_limit(double limit)
{
    if (!is_valid_limit(limit, 0.0, 150.0))
        return;

    config.gpu_temp_limit = limit;
}

void set_storage_alert_limit(double limit)
{
    if (!is_valid_limit(limit, 0.0, 100.0))
        return;

    config.storage_usage_limit = limit;
}