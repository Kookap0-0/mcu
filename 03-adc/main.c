#include "stdlib.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

// const uint LED_PIN = 25;

void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
void help_callback(const char* args);
void mem_callback(const char* args);
void wmem_callback(const char* args);
void get_adc_callback(const char* args);
void get_temp_callback(const char* args);
void tm_start_callback(const char* args);
void tm_stop_callback(const char* args);

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "turn LED on"},
    {"off", led_off_callback, "turn LED off"},
    {"blink", led_blink_callback, "make LED blink"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in milliseconds"},
    {"help", help_callback, "show this help"},
    {"mem", mem_callback, "read memory: mem <hex_address>"},
    {"wmem", wmem_callback, "write memory: wmem <hex_address> <hex_value>"},
    {"get_adc", get_adc_callback, "read ADC voltage on GPIO26"},
    {"get_temp", get_temp_callback, "read internal temperature sensor"},
    {"tm_start", tm_start_callback, "start periodic telemetry (voltage & temperature)"},
    {"tm_stop", tm_stop_callback, "stop periodic telemetry"},
	{NULL, NULL, NULL},
};


void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
    printf("LED turned ON\n");
}

void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
    printf("LED turned OFF\n");
}

void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
    printf("LED blinking\n");
}


void led_blink_set_period_ms_callback(const char* args)
{
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);

    if (period_ms == 0) {
        printf("Error: invalid period. Usage: set_period <milliseconds>\n");
        return;
    }

    led_task_set_blink_period_ms(period_ms);
    printf("Blink period set to %u ms\n", period_ms);
}

void help_callback(const char* args)
{
    printf("\nAvailable commands:\n");
    for (int i = 0; device_api[i].command_name != NULL; i++) {
        printf("  %-12s - %s\n", 
               device_api[i].command_name, 
               device_api[i].command_help);
    }
    printf("\n");
}

void mem_callback(const char* args)
{
    uint32_t addr = 0;
    if (sscanf(args, "%x", &addr) != 1) {
        printf("Error: usage: mem <hex_address>\n");
        return;
    }

    uint32_t value = *(volatile uint32_t*)addr;
    printf("mem[0x%08X] = 0x%08X (%u)\n", addr, value, value);
}

void wmem_callback(const char* args)
{
    uint32_t addr = 0, value = 0;
    if (sscanf(args, "%x %x", &addr, &value) != 2) {
        printf("Error: usage: wmem <hex_address> <hex_value>\n");
        return;
    }
    *(volatile uint32_t*)addr = value;
    printf("wmem[0x%08X] = 0x%08X\n", addr, value);
}

void get_adc_callback(const char* args) {
    float voltage = adc_task_get_voltage();
    printf("%f\n", voltage);
}

void get_temp_callback(const char* args) {
    float temp = adc_task_get_temperature_celsius();
    printf("%f\n", temp);
}

void tm_start_callback(const char* args) {
    adc_task_set_state(ADC_TASK_STATE_RUN);
    printf("Telemetry started\n");
}

void tm_stop_callback(const char* args) {
    adc_task_set_state(ADC_TASK_STATE_IDLE);
    printf("Telemetry stopped\n");
}


int main()
{
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();
    while (1)
    {
        protocol_task_handle(stdio_task_handle());
        led_task_process();
        adc_task_handle();
    }
}
