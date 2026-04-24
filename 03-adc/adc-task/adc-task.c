#include "adc-task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Константы: GPIO26 соответствует каналу ADC0
#define ADC_GPIO_PIN   26
#define ADC_CHANNEL    0
#define TEMP_SENSOR_CHANNEL    4 

// Опорное напряжение (в вольтах)
#define ADC_VREF       3.3f
// Разрядность АЦП: 12 бит → максимальное значение 4095
#define ADC_MAX_COUNTS 4095
#define ADC_TASK_MEAS_PERIOD_US 100000

static adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;  // начальное состояние IDLE
static uint64_t adc_last_measure_us = 0;                  // время последнего измерения

void adc_task_init() {
    adc_init();                     // инициализируем периферию АЦП
    adc_gpio_init(ADC_GPIO_PIN);    // настраиваем GPIO26 как аналоговый вход
    adc_set_temp_sensor_enabled(true);
}

float adc_task_get_voltage() {
    adc_select_input(ADC_CHANNEL);          // выбираем канал 0 (GPIO26)
    uint16_t counts = adc_read();           // считываем 12-битное значение (0..4095)
    float voltage = counts * (ADC_VREF / ADC_MAX_COUNTS);
    return voltage;
}

float adc_task_get_temperature_celsius() {
    adc_select_input(TEMP_SENSOR_CHANNEL);
    uint16_t counts = adc_read();
    float voltage = counts * (ADC_VREF / ADC_MAX_COUNTS);
    float temp_celsius = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temp_celsius;
}

void adc_task_set_state(adc_task_state_t state) {
    adc_state = state;
    if (state == ADC_TASK_STATE_RUN) {
        adc_last_measure_us = 0;
    }
}

void adc_task_handle() {
    if (adc_state != ADC_TASK_STATE_RUN) {
        return;
    }

    uint64_t now = time_us_64();
    if (now - adc_last_measure_us >= ADC_TASK_MEAS_PERIOD_US) {
        adc_last_measure_us = now;

        float voltage = adc_task_get_voltage();
        float temp = adc_task_get_temperature_celsius();

        printf("%f %f\n", voltage, temp);
    }
}