/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT-License
 */

#include <includes.h>

static TaskHandle_t start_task_handle;
static void start_task(void *args);

/**
 * @brief FreeRTOS start up.
 *
 */
void freertos_start(void)
{
    xTaskCreate(start_task, "start_task", 384, NULL, 2, &start_task_handle);
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

    while (1)
        ;
    vTaskDelete(start_task_handle);
}