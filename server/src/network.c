/*
 * network.c
 * This file implements the network server that listens for incoming connections and handles commands to retrieve system metrics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/monitor.h"
#include "../include/network.h"
#include "../include/info.h"

#ifdef SO_REUSEPORT
#define SOCKET_OPT_FLAGS (SO_REUSEADDR | SO_REUSEPORT)
#else
#define SOCKET_OPT_FLAGS SO_REUSEADDR
#endif

// Helper function to build a JSON array of core usages for the GET_CPU_CORES command
void build_cores_json(char *dest, size_t dest_size)
{
    int offset = 0;
    offset += snprintf(dest + offset, dest_size - offset, "[");

    for (int i = 0; i < data.num_cores; i++)
    {
        // Garante que ainda há espaço no buffer antes de escrever
        if (offset >= dest_size - 10)
            break;

        offset += snprintf(dest + offset, dest_size - offset, "%.2f%s",
                           data.cpu_cores[i],
                           (i < data.num_cores - 1) ? "," : "");
    }
    snprintf(dest + offset, dest_size - offset, "]");
}
// Function to calculate CPU usage for each core by reading idle and total times from /proc/stat
void cmd_get_cpu(char *res, size_t size)
{
    snprintf(res, size, "{\"cpu\": %.2f, \"cpu_freq\": %.2f, \"cpu_temp\": %.2f}\n", data.cpu_usage, data.cpu_freq, data.cpu_temp);
}

// Function to build a JSON array of core usages for the GET_CPU_CORES command
void cmd_get_cpu_cores(char *res, size_t size)
{
    int offset = snprintf(res, size, "{\"cores\": ");

    build_cores_json(res + offset, size - offset);

    offset = strlen(res);
    snprintf(res + offset, size - offset, "}\n");
}

// Function to read memory usage and return it in JSON format for the GET_MEM command
void cmd_get_mem(char *res, size_t size)
{
    snprintf(res, size,
             "{\"mem_usage\": %.2f, \"mem_used\": %.2f, \"mem_cached\": %.2f}\n",
             data.mem_usage, data.mem_used, data.mem_cached);
}

// Function to read GPU metrics and return them in JSON format for the GET_GPU command
void cmd_get_gpu(char *res, size_t size)
{
    snprintf(res, size,
             "{\"gpu_temp\": %.2f, \"gpu_vram_used\": %.2f, \"gpu_vram_usage\": %.2f}\n",
             data.gpu_temp, data.gpu_vram_used, data.gpu_vram_usage);
}

// Function to read network metrics and return them in JSON format for the GET_NETWORK command
void cmd_get_network(char *res, size_t size)
{
    snprintf(res, size,
             "{\"net_down\": %.2f, \"net_up\": %.2f, \"net_total_down\": %.2f, \"net_total_up\": %.2f}\n",
             data.net_down, data.net_up, data.net_total_down, data.net_total_up);
}

// Function to read storage metrics and return them in JSON format for the GET_STORAGE command
void cmd_get_storage(char *res, size_t size)
{
    snprintf(res, size,
             "{\"storage_temp\": %.2f, \"storage_read\": %.2f, \"storage_write\": %.2f, \"storage_usage\": %.2f, \"storage_used\": %.2f}\n",
             data.storage_temp, data.storage_read, data.storage_write, data.storage_usage, data.storage_used);
}

// Function to disable alerts and return the new alert status in JSON format for the STOP_ALERT command
void cmd_stop_alert(char *res, size_t size)
{
    data.alert_enabled = 0;
    snprintf(res, size, "{\"alert_enabled\": false}\n");
}

// Function to return all metrics in a single JSON object for the GET_ALL command
void cmd_get_all(char *res, size_t size)
{
    // Build a JSON array of core usages to include in the response
    char cores_buffer[512];
    build_cores_json(cores_buffer, sizeof(cores_buffer));

    // Build the full JSON response with all metrics
    snprintf(res, size,
             "{"
             "\"cpu\": %.2f, "
             "\"cpu_cores\": %s, "
             "\"cpu_freq\": %.2f, "
             "\"cpu_temp\": %.2f, "
             "\"mem_usage\": %.2f, "
             "\"mem_used\": %.2f, "
             "\"mem_cached\": %.2f, "
             "\"alert_enabled\": %d, "
             "\"alert_active\": %d, "
             "\"gpu_temp\": %.2f, "
             "\"gpu_vram_used\": %.2f, "
             "\"gpu_vram_usage\": %.2f, "
             "\"net_down\": %.2f, "
             "\"net_up\": %.2f, "
             "\"net_total_down\": %.2f, "
             "\"net_total_up\": %.2f, "
             "\"storage_temp\": %.2f, "
             "\"storage_read\": %.2f, "
             "\"storage_write\": %.2f, "
             "\"storage_usage\": %.2f, "
             "\"storage_used\": %.2f"
             "}\n",
             data.cpu_usage,
             cores_buffer,
             data.cpu_freq, data.cpu_temp,
             data.mem_usage, data.mem_used, data.mem_cached, data.alert_enabled, data.alert_active,
             data.gpu_temp, data.gpu_vram_used, data.gpu_vram_usage,
             data.net_down, data.net_up, data.net_total_down, data.net_total_up,
             data.storage_temp, data.storage_read, data.storage_write,
             data.storage_usage, data.storage_used);
}

// Function to return the current alert status in JSON format for the GET_ALERT command
void cmd_get_alert(char *res, size_t size)
{
    snprintf(res, size,
             "{\"alert\": %d}\n",
             data.alert_active);
}

// Function to return a summary of key metrics and alert status in JSON format for the GET_STATUS command
void cmd_get_status(char *res, size_t size)
{
    snprintf(res, size,
             "{\"cpu\": %.2f, \"mem_usage\": %.2f, \"mem_used\": %.2f, \"mem_cached\": %.2f, \"mem_usage\": %.2f, \"alert_active\": %d, \"alert_enabled\": %d}\n",
             data.cpu_usage, data.mem_usage, data.mem_used, data.mem_cached, data.mem_usage, data.alert_active, data.alert_enabled);
}

// Function to enable alerts and return the new alert status in JSON format for the ENABLE_ALERT command
void cmd_start_alert(char *res, size_t size)
{
    data.alert_enabled = 1;
    snprintf(res, size, "{\"status\": \"alert enabled\"}\n");
}

// Function to return static system information in JSON format for the GET_INFO command
void cmd_get_info(char *res, size_t size)
{
    snprintf(res, size,
             "{\"cpu_model\": \"%s\", \"cpu_cores\": %d, \"cpu_threads\": %d, \"mem_total\": %.2f, \"gpu_model\": \"%s\", \"gpu_mem_total\": %.2f, \"storage_model\": \"%s\", \"storage_total\": %.2f}\n",
             static_info.cpu_model,
             static_info.cpu_cores,
             static_info.cpu_threads,
             static_info.mem_total,
             static_info.gpu_model,
             static_info.gpu_mem_total,
             static_info.storage_model,
             static_info.storage_total);
}

// Command definitions mapping command strings to their handler functions
Command commands[] = {
    {"GET_ALL", cmd_get_all},
    {"GET_CPU", cmd_get_cpu},
    {"GET_CPU_CORES", cmd_get_cpu_cores},
    {"GET_MEM", cmd_get_mem},
    {"GET_GPU", cmd_get_gpu},
    {"GET_NETWORK", cmd_get_network},
    {"GET_STORAGE", cmd_get_storage},
    {"GET_ALERT", cmd_get_alert},
    {"STOP_ALERT", cmd_stop_alert},
    {"GET_STATUS", cmd_get_status},
    {"ENABLE_ALERT", cmd_start_alert},
    {"GET_INFO", cmd_get_info}};

int num_commands = sizeof(commands) / sizeof(commands[0]);

// Function to handle incoming commands by looking them up in the commands array and calling the corresponding handler function, or returning an error if the command is unknown
void handle_command(const char *cmd, char *response, size_t size)
{
    pthread_mutex_lock(&data_mutex);

    for (int i = 0; i < num_commands; i++)
    {
        if (strcmp(cmd, commands[i].name) == 0)
        {
            commands[i].func(response, size);
            pthread_mutex_unlock(&data_mutex);
            return;
        }
    }

    snprintf(response, size, "{\"error\": \"unknown command\"}\n");

    pthread_mutex_unlock(&data_mutex);
}

// Thread function for the network server that listens for incoming connections, reads commands, and sends back responses
void *thread_network(void *arg)
{
    // Set up a TCP server socket to listen for incoming connections on port 5000 and handle commands in a loop until the server is stopped
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        return NULL;
    }

    // Allow quick reuse of the port after the server is stopped
    setsockopt(server_fd, SOL_SOCKET, SOCKET_OPT_FLAGS, &opt, sizeof(opt));

    // Bind the socket to all interfaces on port 5000 and start listening for incoming connections
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5000);

    // Bind the socket to the specified address and port, and start listening for incoming connections with a backlog of 5
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        return NULL;
    }

    // Start listening for incoming connections with a backlog of 5
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        return NULL;
    }

    printf("[NETWORK] Server listening on port 5000\n");

    while (1)
    {

        // Check if we should keep running before accepting a new connection
        pthread_mutex_lock(&data_mutex);
        int running = data.running;
        pthread_mutex_unlock(&data_mutex);

        if (!running)
            break;

        // Accept a new incoming connection and read the command sent by the client, then handle the command and send back the response before closing the connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            if (data.running)
                perror("Accept failed");
            continue;
        }

        // Read the command from the client, handle it, and send back the response
        char recv_buffer[256];
        char response[2048];

        int bytes = read(new_socket, recv_buffer, sizeof(recv_buffer) - 1);

        if (bytes <= 0)
        {
            close(new_socket);
            continue;
        }

        recv_buffer[bytes] = '\0';

        recv_buffer[strcspn(recv_buffer, "\r\n")] = 0;
        handle_command(recv_buffer, response, sizeof(response));

        send(new_socket, response, strlen(response), 0);

        close(new_socket);
    }

    close(server_fd);
    return NULL;
}