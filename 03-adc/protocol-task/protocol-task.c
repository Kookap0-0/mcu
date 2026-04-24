#include "protocol-task.h"
#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"

static api_t* api = NULL;
static int commands_count = 0;

void protocol_task_init(api_t* device_api)
{
    api = device_api;
    commands_count = 0;
    if (api != NULL) {
        while (api[commands_count].command_name != NULL) {
            commands_count++;
        }
    }
}

void protocol_task_handle(char* command_string)
{
    if (command_string == NULL) {
        return;
    }

    char* space_symbol = strchr(command_string, ' ');
    char* command_args = NULL;
    if (space_symbol) {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    } else {
        command_args = "";
    }

    const char* command_name = command_string;

    printf("command_name: %s\n", command_name);
    printf("command_args: %s\n", command_args);

    for (int i = 0; i < commands_count; i++) {
        if (strcmp(command_name, api[i].command_name) == 0) 
        {
            api[i].command_callback(command_args);
            return;
        }
    }
    
    printf("Unknown command: '%s'\n", command_name);
}