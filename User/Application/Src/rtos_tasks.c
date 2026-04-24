/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT-License
 */

#include <includes.h>
#include <stdio.h>

static TaskHandle_t start_task_handle;
static void start_task(void *args);

static TaskHandle_t daemon_task_handle;
static void daemon_task(void *args);

static TaskHandle_t cmd_task_handle;
static void cmd_task(void *args);

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
    xTaskCreate(daemon_task, "daemon_task", 384, NULL, 3, &daemon_task_handle);
    xTaskCreate(cmd_task, "cmd_task", 384, NULL, 2, &cmd_task_handle);
    taskEXIT_CRITICAL();

    vTaskDelete(start_task_handle);
    vTaskDelay(portMAX_DELAY);
}

/**
 * @brief Motor daemon task, detect motor status.
 *
 * @param args Parameters.
 */
__attribute__((noreturn)) static void daemon_task(void *args)
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
__attribute__((noreturn)) static void cmd_task(void *args)
{
    UNUSED(args);
    for (;;) {
    }
}
