/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "foc_motor.h"

TaskHandle_t foc_vel_loop_task_handle;

__NO_RETURN static void vel_loop_task(void *args)
{
    foc_motor_t *fm = (foc_motor_t *)args;
    TickType_t tick = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&tick, 1);
    }
}

static void foc_adc_cplt_callback(ADC_TypeDef *ADCx)
{
}

/**
 * @brief start FOC control
 *
 * @param fm FOC motor instance
 */
void foc_start(foc_motor_t *fm)
{
    adc1_set_adc_cplt_cb(foc_adc_cplt_callback);
    xTaskCreate(vel_loop_task, "foc vel loop", 384, (void *)fm, 5, &foc_vel_loop_task_handle);
}
