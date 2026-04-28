/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __FOC_MOTOR_H
#define __FOC_MOTOR_H

#include <bsp.h>
#include <stdbool.h>
#include <pid/pid.h>

#include <FreeRTOS.h>
#include <task.h>

/**
 * @brief motor current voltage and temperature
 */
typedef struct {
    /** current of phase */
    float32_t Ia;
    float32_t Ib;
    float32_t Ic;
    /** voltage of phase */
    float32_t Va;
    float32_t Vb;
    float32_t Vc;
    /** for clarke and park */
    float32_t I_alpha;
    float32_t I_beta;
    float32_t sin_val;
    float32_t cos_val;
    /** for SVPWM */
    float32_t V_alpha;
    float32_t V_beta;
    float32_t Vp;
    float32_t Vq;
    /** result of park */
    float32_t Iq;
    float32_t Id;
    /** current offset of phase */
    float32_t Ia_offset;
    float32_t Ib_offset;
    float32_t Ic_offset;
    /** input voltage */
    float32_t Vbus;
} motor_iv_param_t;

#include "clarke_park.h"

/**
 * @brief motor state
 */
typedef enum {
    MOTOR_STATE_IDLE = 0,  /** idle */
    MOTOR_STATE_DETECTING, /** detecting motor parameters */
    MOTOR_STATE_RUNNING,   /** motor running */
    MOTOR_STATE_FAULT,     /** fault occurred */
} motor_state_e;

/**
 * @brief fault status
 */
#define MOTOR_FAULT_NO            (0U)      /** no fault */
#define MOTOR_FAULT_OVER_CURRENT  (1U << 0) /** over current */
#define MOTOR_FAULT_OVER_VOLTAGE  (1U << 1) /** over voltage */
#define MOTOR_FAULT_UNDER_VOLTAGE (1U << 2) /** under voltage */
#define MOTOR_FAULT_OVER_TEMP     (1U << 3) /** over temperature */
#define MOTOR_FAULT_OVER_SPEED    (1U << 4) /** over speed */

/**
 * @brief motor sensor mode
 */
typedef enum {
    MOTOR_SENSOR_MODE_LESS = 0U, /** sensorless */
    MOTOR_SENSOR_MODE_HALL,      /** Hall input */
    MOTOR_SENSOR_MODE_ABI,       /** ABI encoder */
} motor_sensor_mode_e;

/**
 * @brief measure status
 */
typedef enum {
    M_STATE_IDLE = 0U, /** measure state idle */

    /* measure resistance */
    M_STATE_R_START, /** measure resistance start */
    M_STATE_R_LOOP,  /** measuring resistance */
    M_STATE_R_END,   /** measure resistance done */
    /* measure inductance */
    M_STATE_L_START, /** measure inductance start */
    M_STATE_L_LOOP,  /** measuring inductance */
    M_STATE_L_END,   /** measure inductance done */
    /* polarity detect */
    M_STATE_PP_START, /** measure polarity start */
    M_STATE_PP_LOOP,  /** measuring polarity */
    M_STATE_PP_END,   /** measure polarity done */

    M_STATE_ERROR, /** measure error */
} motor_measure_state_t;

/**
 * @brief motor measure parameter
 */
typedef struct {
    float32_t r;       /** phase resistance */
    float32_t l;       /** phase inductance */
    uint8_t pola_num;  /** polarity number */
    bool measure_flag; /** flag of measure, set to 1 if measure done. */
} motor_measure_param_t;

/**
 * @brief FOC control parameters
 */
typedef struct {
    motor_state_e state;             /** current status */
    motor_sensor_mode_e sensor_mode; /** sensor mode */
    uint32_t fault_code;             /** fault status */

    /* measure status */
    motor_state_e measure_state;         /** motor physical measure status */
    motor_measure_param_t measure_param; /** motor physical parameters */

    /* control loop */
    pid_f32_t cur_pid; /** current pid */
    pid_f32_t vel_pid; /** velocity pid */

    motor_iv_param_t iv; /** motor current and voltage */
    float32_t temp;      /** temperature */
} foc_motor_t;

extern TaskHandle_t foc_vel_loop_task_handle;
extern TaskHandle_t foc_measure_task_handle;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FOC_MOTOR_H */
