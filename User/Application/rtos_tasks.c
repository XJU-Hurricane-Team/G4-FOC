/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT-License
 */

#include <bsp.h>

#include <FreeRTOS.h>
#include <task.h>

#include "foc_motor/foc_motor.h"
#include "vofa/vofa.h"

static TaskHandle_t start_task_handle;
static void start_task(void *args);

static TaskHandle_t daemon_task_handle;
static void daemon_task(void *args);

static TaskHandle_t cmd_task_handle;
static void cmd_task(void *args);

foc_motor_t foc_motor;

/**
 * @brief FreeRTOS start up.
 *
 */
void freertos_start(void)
{
    xTaskCreate(start_task, "start_task", 128, NULL, 1, &start_task_handle);
    vTaskStartScheduler();
}

/**
 * @brief Start task. 
 * 
 * @param args Parameters.
 */
static void start_task(void *args)
{
    UNUSED(args);

    taskENTER_CRITICAL();
    // xTaskCreate(daemon_task, "daemon_task", 384, NULL, 2, &daemon_task_handle);
    // xTaskCreate(cmd_task, "cmd_task", 384, NULL, 3, &cmd_task_handle);
    vofa_init(&foc_motor);
    taskEXIT_CRITICAL();

    vTaskDelete(start_task_handle);
    vTaskDelay(portMAX_DELAY);
}

/**
 * @brief Motor daemon task, detect motor status.
 *
 * @param args Parameters.
 */
__NO_RETURN static void daemon_task(void *args)
{
    UNUSED(args);
    for (;;) {
    }
}

/**
 * @brief Command task, control and report.
 *
 * @param args Parameters.
 */
__NO_RETURN static void cmd_task(void *args)
{
    UNUSED(args);
    for (;;) {
    }
}
