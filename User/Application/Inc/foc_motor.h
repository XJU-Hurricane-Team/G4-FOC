/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __FOC_MOTOR_H
#define __FOC_MOTOR_H

typedef enum {
    MOTOR_STATE_IDLE = 0,  /** idle */
    MOTOR_STATE_DETECTING, /** detecting motor parameters */
    MOTOR_STATE_RUNNING,   /** motor running */
    MOTOR_STATE_FAULT,     /** fault occurred */
} foc_motor_state_e;

/**
 * fault status
 */
#define MOTOR_FAULT_NO            (0U)      /** no fault */
#define MOTOR_FAULT_OVER_CURRENT  (1U << 0) /** over current */
#define MOTOR_FAULT_OVER_VOLTAGE  (1U << 1) /** over voltage */
#define MOTOR_FAULT_UNDER_VOLTAGE (1U << 2) /** under voltage */
#define MOTOR_FAULT_OVER_TEMP     (1U << 3) /** over temperature */
#define MOTOR_FAULT_OVER_SPEED    (1U << 4) /** over speed */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FOC_MOTOR_H */
