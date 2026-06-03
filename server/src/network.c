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
#include "../include/config.h"

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
        // Prevent buffer overflow by ensuring we have enough space left in the destination buffer before appending each core usage value
        if (offset >= dest_size - 10)
            break;

        offset += snprintf(dest + offset, dest_size - offset, "%.2f%s",
                           data.cpu_cores[i],
                           (i < data.num_cores - 1) ? "," : "");
    }
    snprintf(dest + offset, dest_size - offset, "]");
}
void cmd_get_cpu(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size, "{\"cpu\": %.2f, \"cpu_freq\": %.2f, \"cpu_temp\": %.2f}\n", data.cpu_usage, data.cpu_freq, data.cpu_temp);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_cpu_cores(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    int offset = snprintf(res, size, "{\"cores\": ");

    build_cores_json(res + offset, size - offset);
    pthread_mutex_unlock(&data_mutex);

    offset = strlen(res);
    snprintf(res + offset, size - offset, "}\n");
}

void cmd_get_mem(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"mem_usage\": %.2f, \"mem_used\": %.2f, \"mem_cached\": %.2f}\n",
             data.mem_usage, data.mem_used, data.mem_cached);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_gpu(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"gpu_temp\": %.2f, \"gpu_vram_used\": %.2f, \"gpu_vram_usage\": %.2f}\n",
             data.gpu_temp, data.gpu_vram_used, data.gpu_vram_usage);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_network(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"net_down\": %.2f, \"net_up\": %.2f, \"net_total_down\": %.2f, \"net_total_up\": %.2f}\n",
             data.net_down, data.net_up, data.net_total_down, data.net_total_up);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_storage(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"storage_temp\": %.2f, \"storage_read\": %.2f, \"storage_write\": %.2f, \"storage_usage\": %.2f, \"storage_used\": %.2f}\n",
             data.storage_temp, data.storage_read, data.storage_write, data.storage_usage, data.storage_used);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_stop_alert(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    data.alert_enabled = 0;
    pthread_mutex_unlock(&data_mutex);

    snprintf(res, size, "{\"alert_enabled\": false}\n");
}

void cmd_get_all(char *res, size_t size)
{
    char cores_buffer[512];
    pthread_mutex_lock(&data_mutex);
    build_cores_json(cores_buffer, sizeof(cores_buffer));

    pthread_mutex_lock(&config_mutex);
    snprintf(res, size,
             "{"
             "\"cpu\": %.2f, \"cpu_cores\": %s, \"cpu_freq\": %.2f, \"cpu_temp\": %.2f, "
             "\"mem_usage\": %.2f, \"mem_used\": %.2f, \"mem_cached\": %.2f, "
             "\"alert_enabled\": %d, \"alert_active\": %d, "
             "\"gpu_temp\": %.2f, \"gpu_vram_used\": %.2f, \"gpu_vram_usage\": %.2f, "
             "\"net_down\": %.2f, \"net_up\": %.2f, \"net_total_down\": %.2f, \"net_total_up\": %.2f, "
             "\"storage_temp\": %.2f, \"storage_read\": %.2f, \"storage_write\": %.2f, "
             "\"storage_usage\": %.2f, \"storage_used\": %.2f, "
             "\"cpu_alert_limit\": %.2f, \"mem_alert_limit\": %.2f, \"cpu_temp_alert_limit\": %.2f, "
             "\"gpu_temp_alert_limit\": %.2f, \"storage_alert_limit\": %.2f"
             "}\n",
             data.cpu_usage, cores_buffer, data.cpu_freq, data.cpu_temp,
             data.mem_usage, data.mem_used, data.mem_cached, data.alert_enabled, data.alert_active,
             data.gpu_temp, data.gpu_vram_used, data.gpu_vram_usage,
             data.net_down, data.net_up, data.net_total_down, data.net_total_up,
             data.storage_temp, data.storage_read, data.storage_write,
             data.storage_usage, data.storage_used,
             config.cpu_usage_limit, config.mem_usage_limit, config.cpu_temp_limit,
             config.gpu_temp_limit, config.storage_usage_limit);
    pthread_mutex_unlock(&config_mutex);
    pthread_mutex_unlock(&data_mutex);
}
void cmd_get_alert(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"alert\": %d}\n",
             data.alert_active);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_status(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    snprintf(res, size,
             "{\"cpu\": %.2f, \"mem_usage\": %.2f, \"mem_used\": %.2f, \"mem_cached\": %.2f, \"mem_usage\": %.2f, \"alert_active\": %d, \"alert_enabled\": %d}\n",
             data.cpu_usage, data.mem_usage, data.mem_used, data.mem_cached, data.mem_usage, data.alert_active, data.alert_enabled);
    pthread_mutex_unlock(&data_mutex);
}

void cmd_start_alert(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
    data.alert_enabled = 1;
    snprintf(res, size, "{\"status\": \"alert enabled\"}\n");
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_info(char *res, size_t size)
{

    pthread_mutex_lock(&data_mutex);
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
    pthread_mutex_unlock(&data_mutex);
}

void cmd_get_config(char *res, size_t size)
{

    snprintf(res, size,
             "{\"cpu_alert_limit\": %.2f, \"mem_alert_limit\": %.2f, \"cpu_temp_alert_limit\": %.2f, \"gpu_temp_alert_limit\": %.2f, \"storage_alert_limit\": %.2f}\n",
             config.cpu_usage_limit,
             config.mem_usage_limit,
             config.cpu_temp_limit,
             config.gpu_temp_limit,
             config.storage_usage_limit);
}

void cmd_set_cpu_alert_limit(char *res, size_t size)
{

    double limit = atof(res);
    set_cpu_alert_limit(limit);
    snprintf(res, size, "{\"cpu_alert_limit\": %.2f}\n", config.cpu_usage_limit);
}

void cmd_set_mem_alert_limit(char *res, size_t size)
{

    double limit = atof(res);
    set_mem_alert_limit(limit);
    snprintf(res, size, "{\"mem_alert_limit\": %.2f}\n", config.mem_usage_limit);
}

void cmd_set_cpu_temp_alert_limit(char *res, size_t size)
{

    double limit = atof(res);
    set_cpu_temp_alert_limit(limit);
    snprintf(res, size, "{\"cpu_temp_alert_limit\": %.2f}\n", config.cpu_temp_limit);
}

void cmd_set_gpu_temp_alert_limit(char *res, size_t size)
{

    double limit = atof(res);
    set_gpu_temp_alert_limit(limit);
    snprintf(res, size, "{\"gpu_temp_alert_limit\": %.2f}\n", config.gpu_temp_limit);
}

void cmd_set_storage_alert_limit(char *res, size_t size)
{

    double limit = atof(res);
    set_storage_alert_limit(limit);
    snprintf(res, size, "{\"storage_alert_limit\": %.2f}\n", config.storage_usage_limit);
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
    {"GET_CONFIG", cmd_get_config},
    {"DISABLE_ALERT", cmd_stop_alert},
    {"GET_STATUS", cmd_get_status},
    {"ENABLE_ALERT", cmd_start_alert},
    {"GET_INFO", cmd_get_info},
    {"SET_CPU_ALERT_LIMIT", cmd_set_cpu_alert_limit},
    {"SET_MEM_ALERT_LIMIT", cmd_set_mem_alert_limit},
    {"SET_CPU_TEMP_ALERT_LIMIT", cmd_set_cpu_temp_alert_limit},
    {"SET_GPU_TEMP_ALERT_LIMIT", cmd_set_gpu_temp_alert_limit},
    {"SET_STORAGE_ALERT_LIMIT", cmd_set_storage_alert_limit}};

int num_commands = sizeof(commands) / sizeof(commands[0]);

// Function to handle incoming commands by looking them up in the command definitions and calling the appropriate handler function
void handle_command(const char *cmd, char *response, size_t size)
{

    char cmd_copy[256];
    strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';
    char *saveptr;

    // Split the command into name and value parts using ":" as a delimiter, where the name is the command string and the value is an optional parameter for SET commands
    char *name = strtok_r(cmd_copy, ":", &saveptr);
    char *val = strtok_r(NULL, ":", &saveptr);

    // Look up the command name in the command definitions and call the handler function
    for (int i = 0; i < num_commands; i++)
    {
        if (strcmp(name, commands[i].name) == 0)
        {
            // SET command (has value)
            if (val != NULL)
            {
                strncpy(response, val, size - 1);
                response[size - 1] = '\0';
            }
            // GET command
            else
            {
                response[0] = '\0';
            }
            commands[i].func(response, size);
            return;
        }
    }

    snprintf(response, size, "{\"error\":\"unknown command\"}\n");
}

// Thread function for each client connection
void *client_handler(void *arg)
{

    int new_socket = *(int *)arg;
    free(arg);

    while (1)
    {
        char recv_buffer[256];
        char response[2048];

        int bytes = read(new_socket, recv_buffer, sizeof(recv_buffer) - 1);

        if (bytes <= 0)
        {
            break; // Connection closed or error
        }

        recv_buffer[bytes] = '\0';
        recv_buffer[strcspn(recv_buffer, "\r\n")] = 0;

        if (strcmp(recv_buffer, "STREAM") == 0)
        {
            // Streaming mode: send data continuously
            while (1)
            {
                pthread_mutex_lock(&data_mutex);
                int running = data.running;
                pthread_mutex_unlock(&data_mutex);
                if (!running)
                    break;

                handle_command("GET_ALL", response, sizeof(response));
                if (send(new_socket, response, strlen(response), 0) <= 0)
                {
                    break; // Error on send (client disconnected)
                }
                usleep(500000); // 500ms polling rate (2Hz) for smoother real-time feel
            }
            break;
        }

        handle_command(recv_buffer, response, sizeof(response));
        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
    return NULL;
}

// Thread function for the network server that listens for incoming connections,
// reads commands, and sends back responses
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

        // Create a new thread to handle the client connection, allowing multiple clients to connect simultaneously without blocking the main server thread
        int *client_sock = malloc(sizeof(int));
        *client_sock = new_socket;
        pthread_t client_thread;

        pthread_create(&client_thread, NULL, client_handler, (void *)client_sock);
        pthread_detach(client_thread);
    }

    close(server_fd);
    return NULL;
}