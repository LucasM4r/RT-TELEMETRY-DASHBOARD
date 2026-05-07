/*
 * info.c
 * This file implements functions to gather system static information such as CPU, memory, GPU, and storage details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include "../include/info.h"

// Get CPU information (model, cores, threads)
void get_cpu_info(SystemInfo *info)
{

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return;

    char line[256];
    info->cpu_threads = 0;
    info->cpu_cores = 0;

    // Parse /proc/cpuinfo to extract CPU model, core count, and thread count
    while (fgets(line, sizeof(line), fp))
    {

        if (strncmp(line, "processor", 9) == 0)
        {
            info->cpu_threads++;
        }

        if (strncmp(line, "cpu cores", 9) == 0)
        {

            char *ptr = strchr(line, ':');
            if (ptr)
            {
                info->cpu_cores = atoi(ptr + 1);
            }
        }

        // Extract CPU model name
        if (strlen(info->cpu_model) == 0 && strncmp(line, "model name", 10) == 0)
        {
            char *name = strchr(line, ':') + 2;
            name[strcspn(name, "\n")] = 0;
            strncpy(info->cpu_model, name, sizeof(info->cpu_model) - 1);
        }
    }
    fclose(fp);

    // If we couldn't find a separate "cpu cores" entry, assume threads = cores
    if (info->cpu_cores == 0)
        info->cpu_cores = info->cpu_threads;
}

// Get total memory by parsing /proc/meminfo
void get_mem_info(SystemInfo *info)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return;

    char line[256];

    // Parse the line to extract total memory in KB and convert to GB
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "MemTotal:", 9) == 0)
        {
            long mem_kb = 0;
            char *p = strchr(line, ':');
            if (p)
            {
                mem_kb = atol(p + 1);

                info->mem_total = (double)mem_kb / (1024.0 * 1024.0);
            }
            break;
        }
    }
    fclose(fp);
}

// Get GPU information using nvidia-smi or fallback methods
static void trim_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

// Get GPU metrics if nvidia-smi is not available (for AMD/Intel GPUs)
void get_gpu_info(SystemInfo *info)
{
    strcpy(info->gpu_model, "Generic GPU");
    info->gpu_mem_total = 4.0;

    FILE *fp = popen(
        "nvidia-smi --query-gpu=name,memory.total --format=csv,noheader,nounits 2>/dev/null",
        "r");

    if (fp)
    {
        char line[256];

        if (fgets(line, sizeof(line), fp))
        {
            char name[128] = {0};
            float mem_mb = 0;

            // Parse the line to extract GPU name and total memory in MB, then convert to GB
            if (sscanf(line, " %127[^,], %f", name, &mem_mb) == 2)
            {
                // Remove trailing newline from GPU name and store it in the info structure
                trim_newline(name);
                strcpy(info->gpu_model, name);
                info->gpu_mem_total = mem_mb / 1024.0; // MB -> GB
            }
        }

        pclose(fp);
        return;
    }

    // If nvidia-smi fails, try to read GPU info from sysfs (works for AMD/Intel)
    FILE *fv = fopen("/sys/class/drm/card0/device/vendor", "r");
    if (fv)
    {
        // Read the vendor ID to determine GPU type and set a generic model name accordingly, then read total VRAM if available
        char vendor_id[16] = {0};
        if (fgets(vendor_id, sizeof(vendor_id), fv))
        {
            trim_newline(vendor_id);

            if (strcmp(vendor_id, "0x1002") == 0)
                strcpy(info->gpu_model, "AMD Radeon GPU");
            else if (strcmp(vendor_id, "0x8086") == 0)
                strcpy(info->gpu_model, "Intel Graphics");
            else
                strcpy(info->gpu_model, "Unknown DRM GPU");
        }
        fclose(fv);
    }

    // Try to read total VRAM from sysfs for AMD/Intel GPUs, using a fallback value if it fails
    FILE *fm = fopen("/sys/class/drm/card0/device/mem_info_vram_total", "r");
    if (fm)
    {
        unsigned long long vram_bytes = 0;

        if (fscanf(fm, "%llu", &vram_bytes) == 1)
        {
            info->gpu_mem_total = (double)vram_bytes / (1024.0 * 1024.0 * 1024.0);
        }

        fclose(fm);
    }
}

// Get storage information by reading the model from sysfs and total size using statvfs
void get_storage_info(SystemInfo *info)
{

    FILE *fp = fopen("/sys/class/block/nvme0n1/device/model", "r");

    if (fp)
    {
        // Read the storage model name from sysfs and store it in the info structure
        char model[128] = {0};
        if (fgets(model, sizeof(model), fp))
        {
            // Remove trailing newline from storage model name and store it in the info structure
            trim_newline(model);
            strncpy(info->storage_model, model, sizeof(info->storage_model) - 1);
        }
        fclose(fp);
    }
    else
    {
        strcpy(info->storage_model, "Generic Storage");
    }

    // Get total storage size using statvfs and convert to GB
    struct statvfs vfs;
    if (statvfs("/", &vfs) == 0)
    {
        // Total de blocos * tamanho do bloco fragmentado
        unsigned long long total_bytes = (unsigned long long)vfs.f_blocks * vfs.f_frsize;
        info->storage_total = (double)total_bytes / (1024.0 * 1024.0 * 1024.0);
    }
}

// Initialize the static SystemInfo structure at startup by gathering all system information
SystemInfo init_system_info()
{
    SystemInfo info;
    memset(&info, 0, sizeof(info));
    get_cpu_info(&info);
    get_mem_info(&info);
    get_gpu_info(&info);
    get_storage_info(&info);
    return info;
}