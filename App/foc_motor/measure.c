/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "foc_motor.h"

TaskHandle_t foc_measure_task_handle;

void measure_task(void *args)
{
    foc_motor_t *fm = (foc_motor_t *)args;

    /* start measure */

    /* measure done */
    vTaskResume(foc_vel_loop_task_handle);
    vTaskDelete(foc_measure_task_handle);
    vTaskDelay(portMAX_DELAY);
}

static void measure_adc_cplt_callback(ADC_TypeDef *ADCx)
{
}

/**
 * @brief start motor parameter measure
 *
 * @param fm FOC motor instance
 */
void motor_measure_start(foc_motor_t *fm)
{
    /* pause foc run */
    tim1_stop_pwm();
    vTaskSuspend(foc_vel_loop_task_handle);

    adc1_set_adc_cplt_cb(measure_adc_cplt_callback);
    xTaskCreate(measure_task, "measure", 384, (void *)fm, 5, &foc_measure_task_handle);
}
