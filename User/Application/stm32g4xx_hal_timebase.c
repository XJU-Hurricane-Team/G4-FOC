/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT-License
 */

#include <stm32g4xx_hal.h>
#include <tim.h>

#define HAL_BASE_TIME            TIM6
#define HAL_BASE_TIME_INIT()     MX_TIM6_Init()
#define HAL_BASE_TIME_IRQn       TIM6_DAC_IRQn
#define HAL_BASE_TIME_IRQHandler TIM6_DAC_IRQHandler

/**
 * @brief  This function configures the TIM as a time base source.
 *         The time source is configured  to have 1ms time base with a dedicated
 *         Tick interrupt priority.
 * @note   This function is called  automatically at the beginning of program after
 *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
 * @param  TickPriority: Tick interrupt priority.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    HAL_BASE_TIME_INIT();

    uwTickPrio = TickPriority;
    if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
        NVIC_SetPriority(HAL_BASE_TIME_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TickPriority, 0));
    }

    LL_TIM_EnableIT_UPDATE(HAL_BASE_TIME);
    LL_TIM_EnableCounter(HAL_BASE_TIME);

    return HAL_OK;
}

/**
 * @brief  Suspend Tick increment.
 * @note   Disable the tick increment by disabling HAL timer base update interrupt.
 */
void HAL_SuspendTick(void)
{
    LL_TIM_DisableIT_UPDATE(HAL_BASE_TIME);
}

/**
 * @brief  Resume Tick increment.
 * @note   Enable the tick increment by Enabling HAL timer base update interrupt.
 */
void HAL_ResumeTick(void)
{
    /* Enable TIM Update interrupt */
    LL_TIM_EnableIT_UPDATE(HAL_BASE_TIME);
}

/**
 * @brief This function handles HAL base timer global interrupt.
 */
void HAL_BASE_TIME_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(HAL_BASE_TIME) && LL_TIM_IsEnabledIT_UPDATE(HAL_BASE_TIME)) {
        LL_TIM_ClearFlag_UPDATE(HAL_BASE_TIME);
        HAL_IncTick();
    }
}