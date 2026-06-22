/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "foc_motor.h"


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

    adc1_set_adc_cplt_cb(measure_adc_cplt_callback);
}
