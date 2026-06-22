/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "foc_motor.h"

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
}
