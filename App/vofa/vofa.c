/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "vofa.h"
#include <bsp.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

static SemaphoreHandle_t tx_sem;
#if (MC_CFG_VOFA_SEND_PERIOD != 0)
static TaskHandle_t vofa_tx_task_handle;
#endif /* MC_CFG_VOFA_SEND_PERIOD != 0 */

static EventGroupHandle_t rx_event;
static TaskHandle_t vofa_rx_task_handle;
#define RX_BUF_SIZE (256)
static char rx_buf[RX_BUF_SIZE];

#define RX_EVENT_IDLE  (1U << 0)
#define RX_EVENT_FULL  (1U << 1)
#define RX_EVENT_ERROR (1U << 2)

/**
 * @brief Sends data over UART using DMA.
 * @param[in] data Pointer to the data buffer to send.
 * @param[in] len Length of the data to send in bytes.
 * @note This function blocks until the previous DMA transfer is complete.
 */
static void uart_dma_send(void *data, uint32_t len)
{
    xSemaphoreTake(tx_sem, portMAX_DELAY);

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, len);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)data);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&USART1->TDR);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_USART_EnableDMAReq_TX(USART1);
}

/**
 * @brief Starts UART reception using DMA in circular mode.
 * @note This function configures DMA channel 2 for USART1 RX and enables IDLE line detection.
 */
static void uart_start_rx_dma(void)
{
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)&USART1->RDR);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)rx_buf);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, RX_BUF_SIZE);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_2);
    LL_USART_EnableIT_IDLE(USART1);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_USART_EnableDMAReq_RX(USART1);
}

/**
 * @brief Stops UART reception DMA.
 * @note Disables DMA channel 2 and associated interrupts for USART1 RX.
 */
static void uart_stop_rx_dma(void)
{
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_2);
    LL_USART_DisableIT_IDLE(USART1);

    LL_USART_DisableDMAReq_RX(USART1);
}

/**
 * @brief Parses received VOFA data.
 * @param[in] str Pointer to the received data string.
 * @param[in] len Length of the received data.
 * @note This function is currently a placeholder and does not implement any parsing logic.
 */
static void vofa_rx_parse(char *str, uint32_t len)
{
    /* TODO: Implement VOFA data parsing logic here */
}

/**
 * @brief FreeRTOS task for handling VOFA UART reception.
 *
 * @param args Not used in this task.
 * @note This task continuously monitors for UART RX events (IDLE, FULL, ERROR)
 *       and restarts the DMA reception after processing.
 */
__NO_RETURN static void vofa_rx_task(void *args)
{
    EventBits_t event_bits;

    uart_start_rx_dma();
    for (;;) {
        /* Wait indefinitely for any RX event */
        event_bits = xEventGroupWaitBits(rx_event,
                                         RX_EVENT_IDLE | RX_EVENT_FULL | RX_EVENT_ERROR,
                                         pdTRUE,  /* Clear bits on exit */
                                         pdFALSE, /* Don't wait for all bits */
                                         portMAX_DELAY);

        if (event_bits & RX_EVENT_IDLE) {
            /* IDLE line detected, data received */
            vofa_rx_parse(rx_buf, RX_BUF_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2));
        } else if (event_bits & RX_EVENT_FULL) {
            /* Buffer full, process data */
            vofa_rx_parse(rx_buf, RX_BUF_SIZE);
        } else if (event_bits & RX_EVENT_ERROR) {
            /* DMA transfer error, restart DMA */
            uart_stop_rx_dma();
            memset(rx_buf, 0, sizeof(rx_buf));
            uart_start_rx_dma();
            continue;
        }

        /* restart receive */
        memset(rx_buf, 0, sizeof(rx_buf));
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
        LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, RX_BUF_SIZE);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
    }
}

#if (MC_CFG_VOFA_SEND_PERIOD != 0)
/**
 * @brief FreeRTOS task for periodically sending motor data via VOFA.
 * @param[in] args Pointer to the `foc_motor_t` instance containing motor data.
 * @note The sending period is configured by `MC_CFG_VOFA_SEND_PERIOD`.
 */
__NO_RETURN static void vofa_tx_task(void *args)
{
    foc_motor_t *fm = (foc_motor_t *)args;
    for (;;) {
        vofa_foc_data_send(fm);

        vTaskDelay(pdMS_TO_TICKS(MC_CFG_VOFA_SEND_PERIOD));
    }
}
#endif /* MC_CFG_VOFA_SEND_PERIOD != 0 */

/**
 * @brief Sends FOC motor data to VOFA.
 * @param[in] fm Pointer to the `foc_motor_t` instance whose data is to be sent.
 * @note This function sends the `fm->iv` structure followed by a VOFA EOF marker.
 */
void vofa_foc_data_send(foc_motor_t *fm)
{
    /* keep it in SRAM for DMA */
    static uint32_t VOFA_EOF = 0x7f80000;

    uart_dma_send(&fm->iv, sizeof(fm->iv));
    uart_dma_send(&VOFA_EOF, sizeof(VOFA_EOF));
}

/**
 * @brief Initializes the VOFA communication module.
 * @param[in] fm Pointer to the `foc_motor_t` instance to be used by the TX task.
 * @note This function creates necessary FreeRTOS semaphores, event groups, and tasks
 *       for VOFA transmission and reception.
 */
void vofa_init(foc_motor_t *fm)
{
    tx_sem = xSemaphoreCreateBinary();
    /* Give the semaphore initially so the first send can proceed */
    xSemaphoreGive(tx_sem);

    rx_event = xEventGroupCreate();
    xTaskCreate(vofa_rx_task, "vofa rx", 128, NULL, 2, &vofa_rx_task_handle);
#if (MC_CFG_VOFA_SEND_PERIOD != 0)
    xTaskCreate(vofa_tx_task, "vofa tx", 128, (void *)fm, 2, &vofa_tx_task_handle);
#endif /* MC_CFG_VOFA_SEND_PERIOD != 0 */
}

/**
 * @brief Interrupt handler for DMA1 Channel 1 (USART1 TX).
 * @note This handler manages the completion and error flags for the UART TX DMA transfer.
 *       It releases the `tx_sem` semaphore upon successful transmission.
 */
void DMA1_Channel1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_1);
        LL_DMA_ClearFlag_TC1(DMA1);

        /* Wait until the last byte is shifted out of the transmit register */
        while (LL_USART_IsActiveFlag_BUSY(USART1))
            ;
        xSemaphoreGiveFromISR(tx_sem, &xHigherPriorityTaskWoken);
    }
    if (LL_DMA_IsActiveFlag_TE1(DMA1)) {
        /* Transfer Error occurred */
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_1);
        LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_1);
        LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_1);

        LL_DMA_ClearFlag_TE1(DMA1);
        xSemaphoreGiveFromISR(tx_sem, &xHigherPriorityTaskWoken); /* Release semaphore even on error */
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Interrupt handler for DMA1 Channel 2 (USART1 RX).
 * @note This handler manages the completion and error flags for the UART RX DMA transfer.
 *       It sets event bits in `rx_event` for full buffer or transfer errors.
 */
void DMA1_Channel2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (LL_DMA_IsActiveFlag_HT2(DMA1)) {
        LL_DMA_ClearFlag_HT2(DMA1);
    }
    if (LL_DMA_IsActiveFlag_TC2(DMA1)) {
        LL_DMA_ClearFlag_TC2(DMA1);
        xEventGroupSetBitsFromISR(rx_event, RX_EVENT_FULL, &xHigherPriorityTaskWoken);
    }
    if (LL_DMA_IsActiveFlag_TE2(DMA1)) {
        /* Transfer Error occurred */
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_2);
        LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_2);

        LL_DMA_ClearFlag_TE2(DMA1);
        xEventGroupSetBitsFromISR(rx_event, RX_EVENT_ERROR, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Interrupt handler for USART1 global interrupt.
 * @note This handler primarily detects UART IDLE line and Overrun errors,
 *       setting corresponding event bits in `rx_event`.
 */
void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (LL_USART_IsActiveFlag_IDLE(USART1)) {
        LL_USART_ClearFlag_IDLE(USART1);
        xEventGroupSetBitsFromISR(rx_event, RX_EVENT_IDLE, &xHigherPriorityTaskWoken);
    }
    if (LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
        xEventGroupSetBitsFromISR(rx_event, RX_EVENT_ERROR, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
