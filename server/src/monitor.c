/*
 * monitor.c
 * Thread-based system monitoring module.
 * Collects system metrics and updates the shared SystemData structure.
 */

#include "../include/monitor.h"
#include "../include/info.h"
#include "../include/config.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/statvfs.h>

// Helper function to locate the NVMe temperature sensor path
void get_nvme_temp_path(char *path, size_t size)
{
    char name_path[64];
    char name_content[32];

    for (int i = 0; i < 10; i++)
    {
        // Check hwmon entries for NVMe devices
        snprintf(name_path, sizeof(name_path),
                 "/sys/class/hwmon/hwmon%d/name", i);

        FILE *fp = fopen(name_path, "r");

        if (!fp)
            continue;

        // Read the name of the hwmon device
        if (fgets(name_content, sizeof(name_content), fp))
        {
            name_content[strcspn(name_content, "\n")] = '\0';

            // If the name contains "nvme", we assume this is the correct sensor
            if (strstr(name_content, "nvme"))
            {
                snprintf(path, size,
                         "/sys/class/hwmon/hwmon%d/temp1_input", i);

                fclose(fp);
                return;
            }
        }

        fclose(fp);
    }
    // If we can't find a specific NVMe sensor, leave the path empty
    path[0] = '\0';
}

// Function to read CPU times (idle and total) for usage calculation
void get_cpu_times(long *idle, long *total)
{
    // Read CPU times from /proc/stat
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp)
    {
        perror("Error when trying to open /proc/stat");
        return;
    }

    char cpu[10];
    long user, nice, system, idle_time, iowait, irq, softirq, steal;

    if (fscanf(fp, "%s %ld %ld %ld %ld %ld %ld %ld %ld",
               cpu, &user, &nice, &system, &idle_time,
               &iowait, &irq, &softirq, &steal) < 9)
    {
        perror("Error when trying to read /proc/stat");
    }
    *idle = idle_time + iowait;
    *total = user + nice + system + idle_time + iowait + irq + softirq + steal;

    fclose(fp);
}

// Function to calculate overall CPU usage percentage
void read_cpu_percent(SystemData *data)
{
    // To calculate CPU usage, we read idle and total times at two intervals
    long idle1, total1, idle2, total2;

    get_cpu_times(&idle1, &total1);

    usleep(100000);

    get_cpu_times(&idle2, &total2);

    // Calculate the difference in idle and total times
    long delta_idle = idle2 - idle1;
    long delta_total = total2 - total1;

    // Avoid division by zero
    if (delta_total == 0)
    {
        data->cpu_usage = 0.0;
        return;
    }

    // (Total - Idle) / Total
    data->cpu_usage = (double)(delta_total - delta_idle) / delta_total * 100.0;
}

// Function to read the current CPU frequency (average across cores)
void get_cpu_freq(SystemData *data)
{
    float sum = 0;
    int count = 0;
    char path[128];

    // Read the current frequency of each CPU core and average them
    for (int i = 0; i < MAX_CORES; i++)
    {

        // Try to read the current frequency for each core
        snprintf(path, sizeof(path),
                 "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", i);

        FILE *fp = fopen(path, "r");
        if (!fp)
            continue;

        long freq_khz;

        // If successfully read the frequency, add it to the sum
        if (fscanf(fp, "%ld", &freq_khz) == 1)
        {
            sum += freq_khz / 1000000.0;
            count++;
        }

        fclose(fp);
    }

    data->cpu_freq = count > 0 ? sum / count : 0.0;
}

// Function to read the CPU temperature (tries multiple common paths)
void get_cpu_temp(SystemData *data)
{
    char path[64];

    // Try to read CPU temperature from common thermal zones
    for (int i = 0; i < MAX_THERMAL_ZONES; i++)
    {

        // Check thermal zones for CPU temperature
        snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%d/temp", i);
        FILE *fp = fopen(path, "r");

        if (!fp)
            continue;

        int temp_milli;

        // If successfully read the temperature, convert from millidegrees to degrees Celsius
        if (fscanf(fp, "%d", &temp_milli) == 1 && temp_milli > 0)
        {
            fclose(fp);
            data->cpu_temp = temp_milli / 1000.0;
            return;
        }
        fclose(fp);
    }
    data->cpu_temp = 0.0;
}

// Function to read per-core CPU usage by comparing idle and total times for each core
void get_cores_usage(SystemData *data)
{
    // The CPU cores idle and total times are read from /proc/stat
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp)
        return;

    char line[256];

    if (!fgets(line, sizeof(line), fp))
    {
        fclose(fp);
        return;
    }

    int i = 0;

    // Read lines starting with "cpu" followed by a digit (cpu0, cpu1, etc.) to get per-core stats
    while (fgets(line, sizeof(line), fp) && i < MAX_CORES)
    {
        // We want to skip the first line which is the aggregate "cpu" line and only process lines like "cpu0", "cpu1", etc.
        if (strncmp(line, "cpu", 3) != 0 || line[3] == ' ')
            break;

        long user, nice, system, idle, iowait, irq, softirq, steal;

        // Parse the CPU times for this core and calculate usage based on the difference from the last read
        if (sscanf(line, "cpu%*d %ld %ld %ld %ld %ld %ld %ld %ld",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) >= 8)
        {
            long current_idle = idle + iowait;
            long current_total = user + nice + system + idle + iowait + irq + softirq + steal;

            long diff_idle = current_idle - data->last_idle_cores[i];
            long diff_total = current_total - data->last_total_cores[i];

            if (diff_total > 0)
            {
                // Calculate the usage percentage for this core and store it in the SystemData structure
                data->cpu_cores[i] = (double)(diff_total - diff_idle) / diff_total * 100.0;
            }
            else
            {
                data->cpu_cores[i] = 0.0;
            }

            // Update the last idle and total times for this core for the next calculation
            data->last_idle_cores[i] = current_idle;
            data->last_total_cores[i] = current_total;
            i++;
        }
    }

    // Store the number of cores we read
    data->num_cores = i;
    fclose(fp);
}

// Fallback function to get GPU metrics if nvidia-smi is not available (for NVIDIA GPUs) or if it fails
void get_gpu_metrics_fallback(SystemData *data)
{

    // Try to read GPU temperature from common thermal zones (some systems expose GPU temp here)
    for (int i = 0; i < MAX_THERMAL_ZONES; i++)
    {
        char type_path[64];
        char temp_path[64];
        char type[64];

        snprintf(type_path, sizeof(type_path),
                 "/sys/class/thermal/thermal_zone%d/type", i);

        FILE *f_type = fopen(type_path, "r");

        if (!f_type)
            continue;

        if (!fgets(type, sizeof(type), f_type))
        {
            fclose(f_type);
            continue;
        }

        fclose(f_type);

        // Skip zones that are not GPU related
        if (!strstr(type, "gpu") &&
            !strstr(type, "amdgpu"))
        {
            continue;
        }

        // If we find a GPU-related thermal zone, try to read its temperature
        snprintf(temp_path, sizeof(temp_path),
                 "/sys/class/thermal/thermal_zone%d/temp", i);

        FILE *f_temp = fopen(temp_path, "r");

        if (!f_temp)
            continue;

        int temp_raw;

        // If we successfully read the temperature, convert it from millidegrees to degrees Celsius and store it in the SystemData structure
        if (fscanf(f_temp, "%d", &temp_raw) == 1)
        {
            data->gpu_temp = (double)temp_raw / 1000.0;
            fclose(f_temp);
            break;
        }

        fclose(f_temp);
    }

    FILE *f_vram = fopen("/sys/class/drm/card0/device/mem_info_vram_used", "r");

    // If we successfully read the VRAM usage, convert it to GB and calculate the percentage based on the total VRAM from static_info
    if (f_vram)
    {
        long long vram_bytes;
        if (fscanf(f_vram, "%lld", &vram_bytes) == 1)
        {
            data->gpu_vram_used = (double)vram_bytes / (1024.0 * 1024.0 * 1024.0);

            double total_vram = (static_info.gpu_mem_total > 0) ? static_info.gpu_mem_total : 4.0;
            data->gpu_vram_usage = (data->gpu_vram_used / total_vram) * 100.0;
        }
        fclose(f_vram);
    }
}

// Function to get GPU metrics using nvidia-smi command (for NVIDIA GPUs)
// and fallback methods if it fails
void get_gpu_metrics(SystemData *data)
{
    // Reset values to avoid keeping stale data if all methods fail
    data->gpu_temp = 0.0;
    data->gpu_vram_used = 0.0;
    data->gpu_vram_usage = 0.0;

    // Try to get GPU metrics using nvidia-smi
    FILE *fp = popen(
        "nvidia-smi --query-gpu=temperature.gpu,memory.used "
        "--format=csv,noheader,nounits 2>/dev/null",
        "r");

    char buffer[128];

    // If popen fails, use fallback immediately
    if (!fp)
    {
        get_gpu_metrics_fallback(data);
        return;
    }

    // Parse GPU temperature and VRAM usage
    if (fgets(buffer, sizeof(buffer), fp) &&
        sscanf(buffer, "%lf, %lf",
               &data->gpu_temp,
               &data->gpu_vram_used) == 2)
    {
        // Convert VRAM usage from MB to GB
        data->gpu_vram_used /= 1024.0;

        // Calculate VRAM usage percentage only if total VRAM is valid
        if (static_info.gpu_mem_total > 0)
        {
            data->gpu_vram_usage =
                (data->gpu_vram_used /
                 static_info.gpu_mem_total) *
                100.0;
        }
        else
        {
            data->gpu_vram_usage = 0.0;
        }

        pclose(fp);
        return;
    }

    // Close process before fallback
    pclose(fp);

    get_gpu_metrics_fallback(data);
}

// Function to get network metrics by reading /proc/net/dev for bandwidth and using ping for latency
void get_network_metrics(SystemData *data)
{
    static long long last_recv = 0, last_sent = 0;
    static struct timeval last_time;

    // Read network interface statistics from /proc/net/dev to calculate download and upload speeds
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp)
        return;

    char line[256];
    long long recv = 0, sent = 0;

    // Skip the first two lines which are headers
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    // Sum up the received and transmitted bytes across all interfaces except loopback
    while (fgets(line, sizeof(line), fp))
    {
        // Skip loopback interface
        if (strstr(line, "lo:"))
            continue;

        // Parse the line to extract received and transmitted bytes
        char *ptr = strchr(line, ':');
        if (ptr == NULL)
            continue;

        long long r_bytes = 0, t_bytes = 0;

        // The format of /proc/net/dev is:
        // Inter-|   Receive                                                |  Transmit
        //  face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
        if (sscanf(ptr + 1, "%lld %*d %*d %*d %*d %*d %*d %*d %lld", &r_bytes, &t_bytes) >= 2)
        {
            recv += r_bytes;
            sent += t_bytes;
        }
    }
    fclose(fp);

    struct timeval now;
    gettimeofday(&now, NULL);

    // Calculate elapsed time since last read to compute speeds
    double elapsed = (now.tv_sec - last_time.tv_sec) + (now.tv_usec - last_time.tv_usec) / 1000000.0;

    // Calculate download and upload speeds in Mbps based on the difference in bytes and elapsed time
    if (last_recv > 0 && elapsed > 0)
    {
        data->net_down = ((recv - last_recv) * 8.0) / elapsed / 1000000.0;
        data->net_up = ((sent - last_sent) * 8.0) / elapsed / 1000000.0;
    }

    // Update total data transferred in GB
    data->net_total_down = (double)recv / (1024.0 * 1024.0 * 1024.0);
    data->net_total_up = (double)sent / (1024.0 * 1024.0 * 1024.0);

    last_recv = recv;
    last_sent = sent;
    last_time = now;
}

// Get storage read/write speeds from /proc/diskstats
void get_storage_io(SystemData *data)
{
    static unsigned long long last_read_sectors = 0;
    static unsigned long long last_write_sectors = 0;
    static struct timeval last_time;

    unsigned long long r_sectors = 0;
    unsigned long long w_sectors = 0;

    FILE *fp = fopen("/proc/diskstats", "r");

    if (!fp)
        return;

    char line[256];

    // Find the main storage device entry
    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "nvme0n1 "))
        {
            sscanf(line,
                   "%*u %*u %*s "
                   "%*u %*u %llu %*u "
                   "%*u %*u %llu",
                   &r_sectors,
                   &w_sectors);

            break;
        }
    }

    fclose(fp);

    struct timeval now;
    gettimeofday(&now, NULL);

    double elapsed =
        (now.tv_sec - last_time.tv_sec) +
        (now.tv_usec - last_time.tv_usec) / 1000000.0;

    // Avoid invalid calculations on first read
    if (last_read_sectors > 0 && elapsed > 0)
    {
        // Calculate read/write speeds in MB/s based on the difference in sectors and elapsed time (assuming 512 bytes per sector)
        data->storage_read =
            ((r_sectors - last_read_sectors) * 512.0) /
            elapsed / (1024.0 * 1024.0);

        data->storage_write =
            ((w_sectors - last_write_sectors) * 512.0) /
            elapsed / (1024.0 * 1024.0);
    }
    else
    {
        data->storage_read = 0.0;
        data->storage_write = 0.0;
    }

    last_read_sectors = r_sectors;
    last_write_sectors = w_sectors;
    last_time = now;
}

// Get storage usage using statvfs
void get_storage_usage(SystemData *data)
{
    struct statvfs vfs;

    if (statvfs("/", &vfs) != 0)
        return;

    // Calculate total, free, and used storage based on the filesystem statistics and update the SystemData structure with usage in GB and percentage
    unsigned long long total =
        (unsigned long long)vfs.f_blocks * vfs.f_frsize;

    unsigned long long free =
        (unsigned long long)vfs.f_bfree * vfs.f_frsize;

    unsigned long long used = total - free;

    // Avoid division by zero and calculate used storage in GB and usage percentage
    if (total > 0)
    {
        data->storage_used =
            (double)used / (1024.0 * 1024.0 * 1024.0);

        data->storage_usage =
            ((double)used / (double)total) * 100.0;
    }
    else
    {
        data->storage_used = 0.0;
        data->storage_usage = 0.0;
    }
}

// Get NVMe storage temperature
void get_storage_temp(SystemData *data)
{
    static char temp_path[64] = "";

    // Locate NVMe temperature sensor only once
    if (temp_path[0] == '\0')
    {
        get_nvme_temp_path(temp_path, sizeof(temp_path));
    }

    if (temp_path[0] == '\0')
    {
        data->storage_temp = 0.0;
        return;
    }

    FILE *ft = fopen(temp_path, "r");

    if (!ft)
    {
        data->storage_temp = 0.0;
        return;
    }

    int temp_milli;

    if (fscanf(ft, "%d", &temp_milli) == 1)
    {
        data->storage_temp = temp_milli / 1000.0;
    }
    else
    {
        data->storage_temp = 0.0;
    }

    fclose(ft);
}

// Main storage metrics wrapper
void get_storage_metrics(SystemData *data)
{
    get_storage_io(data);
    get_storage_usage(data);
    get_storage_temp(data);
}

// Thread function to continuously monitor CPU metrics and update the shared SystemData structure with mutex protection
void *thread_cpu_monitor(void *arg)
{

    static SystemData temp;
    while (1)
    {

        read_cpu_percent(&temp);
        get_cpu_freq(&temp);
        get_cpu_temp(&temp);
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        pthread_mutex_lock(&data_mutex);

        get_cores_usage(&data);

        data.cpu_usage = temp.cpu_usage;
        data.cpu_freq = temp.cpu_freq;
        data.cpu_temp = temp.cpu_temp;

        if (data.cpu_usage > 90.0)
        {
            pthread_cond_signal(&alert_cond);
        }

        pthread_mutex_unlock(&data_mutex);

        usleep(500000);
    }

    return NULL;
}

// Function to read memory usage by parsing /proc/meminfo and calculating used, cached, and usage percentage, then updating the SystemData structure
void read_memory_usage(SystemData *data)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
    {
        perror("Error opening /proc/meminfo");
        return;
    }

    long total = 0, free = 0, buffers = 0, cached = 0, reclaimable = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "MemTotal: %ld kB", &total) == 1)
            continue;
        if (sscanf(line, "MemFree: %ld kB", &free) == 1)
            continue;
        if (sscanf(line, "Buffers: %ld kB", &buffers) == 1)
            continue;
        if (sscanf(line, "Cached: %ld kB", &cached) == 1)
            continue;
        if (sscanf(line, "SReclaimable: %ld kB", &reclaimable) == 1)
            continue;
    }
    fclose(fp);

    if (total > 0)
    {
        long used_real_kb = total - (free + buffers + cached + reclaimable);
        data->mem_used = (double)used_real_kb / (1024.0 * 1024.0);
        data->mem_cached = (double)(buffers + cached + reclaimable) / (1024.0 * 1024.0);
        data->mem_usage = (double)used_real_kb / total * 100.0;
    }
}
// Thread function to continuously monitor memory usage and update the shared SystemData structure with mutex protection
void *thread_memory_monitor(void *arg)
{
    static SystemData temp;

    while (1)
    {

        read_memory_usage(&temp);
        // Lock to check if we should keep running
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        pthread_mutex_lock(&data_mutex);
        data.mem_used = temp.mem_used;
        data.mem_cached = temp.mem_cached;
        data.mem_usage = temp.mem_usage;
        if (data.mem_usage > 95.0)
        {
            pthread_cond_signal(&alert_cond);
        }
        pthread_mutex_unlock(&data_mutex);

        sleep(1);
    }
    return NULL;
}

// Thread to check alert conditions and update alert status
void *thread_alert_handler(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        pthread_mutex_lock(&data_mutex);

        if (data.alert_enabled)
        {
            // Check if any of the critical conditions are met and update the alert_active status accordingly
            bool cpu_critic = data.cpu_usage > config.cpu_usage_limit;
            bool ram_critic = data.mem_usage > config.mem_usage_limit;
            bool temp_critic = data.cpu_temp > config.cpu_temp_limit || data.gpu_temp > config.gpu_temp_limit;
            bool disk_critic = data.storage_usage > config.storage_usage_limit;

            if (cpu_critic || ram_critic || temp_critic || disk_critic)
            {
                if (data.alert_active == 0)
                {
                    printf("\n[!!!] SYSTEM IN CRITICAL STATE [!!!]\n");
                }
                data.alert_active = 1;
            }
            else
            {
                data.alert_active = 0;
            }
        }
        else
        {
            data.alert_active = 0;
        }

        pthread_mutex_unlock(&data_mutex);
        usleep(500000);
    }
    return NULL;
}

// Thread to get GPU metrics (temperature, VRAM usage)
void *thread_gpu_monitor(void *arg)
{

    static SystemData temp;
    while (1)
    {

        // Lock to check if we should keep running
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        get_gpu_metrics(&temp);

        // Update shared data with mutex protection
        pthread_mutex_lock(&data_mutex);
        data.gpu_temp = temp.gpu_temp;
        data.gpu_vram_used = temp.gpu_vram_used;
        data.gpu_vram_usage = temp.gpu_vram_usage;

        if (data.gpu_vram_usage > 90.0)
        {
            pthread_cond_signal(&alert_cond);
        }
        pthread_mutex_unlock(&data_mutex);

        sleep(1);
    }
    return NULL;
}

// Thread to get network metrics (download/upload speed, total data, latency)
void *thread_network_monitor(void *arg)
{
    static SystemData temp;

    while (1)
    {

        // Lock to check if we should keep running
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        get_network_metrics(&temp);

        // Update shared data with mutex protection
        pthread_mutex_lock(&data_mutex);
        data.net_down = temp.net_down;
        data.net_up = temp.net_up;
        data.net_total_down = temp.net_total_down;
        data.net_total_up = temp.net_total_up;
        data.latency = temp.latency;
        pthread_mutex_unlock(&data_mutex);

        sleep(1);
    }

    return NULL;
}

// Thread to get storage metrics (read/write speed, usage, temperature)
void *thread_storage_monitor(void *arg)
{
    static SystemData temp;

    while (1)
    {
        // Lock to check if we should keep running
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        get_storage_metrics(&temp);

        // Update shared data with mutex protection
        pthread_mutex_lock(&data_mutex);
        data.storage_read = temp.storage_read;
        data.storage_write = temp.storage_write;
        data.storage_temp = temp.storage_temp;
        data.storage_used = temp.storage_used;
        data.storage_usage = temp.storage_usage;
        pthread_mutex_unlock(&data_mutex);

        sleep(1);
    }
    return NULL;
}