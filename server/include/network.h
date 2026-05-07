/*
 * network.h
 * This header defines the Command structure and function prototypes for handling network commands to retrieve system metrics.
 * It also declares the thread function for the network server.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>
#include "../include/info.h"

typedef void (*command_func)(char *response, size_t size);

typedef struct
{
    const char *name;
    command_func func;
} Command;

void build_cores_json(char *dest, size_t dest_size);
// Function prototypes for command handlers
void cmd_get_cpu(char *res, size_t size);
void cmd_get_cpu_cores(char *res, size_t size);
void cmd_get_mem(char *res, size_t size);
void cmd_stop_alert(char *res, size_t size);
void cmd_get_gpu(char *res, size_t size);
void cmd_get_network(char *res, size_t size);
void cmd_get_storage(char *res, size_t size);
void cmd_get_all(char *res, size_t size);
void cmd_get_info(char *res, size_t size);
void cmd_get_alert(char *res, size_t size);
void cmd_get_status(char *res, size_t size);
void cmd_start_alert(char *res, size_t size);

// Command handler function
void handle_command(const char *cmd, char *response, size_t size);

// Thread function for the network server
void *thread_network(void *arg);

#endif