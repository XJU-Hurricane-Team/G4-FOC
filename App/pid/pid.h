/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __PID_H
#define __PID_H

#include <arm_math.h>

/**
 * @brief pid controller with anti-
 */
typedef struct {
    arm_pid_instance_f32 arm_pid; /** arm pid instance */
    float32_t i_max;              /** maximum integral output */
    float32_t out_max;            /** maximum output */
} pid_f32_t;

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Perform PID calculation to resist integral saturation.
 *
 * @param pid PID instance
 * @param target target value
 * @param current current value
 * @return PID calculation result
 */
__STATIC_FORCEINLINE float32_t pid_f32_calc(pid_f32_t *pid, float32_t target, float32_t current)
{
    float32_t error = target - current;
    arm_pid_instance_f32 *S = &pid->arm_pid;
    float32_t last_out = S->state[2];

    float32_t out = arm_pid_f32(S, error);
    if (out > pid->out_max) {
        if (error > 0) {
            S->state[2] = last_out;
        }
        out = pid->out_max;
    } else if (out < -pid->out_max) {
        if (error < 0) {
            S->state[2] = last_out;
        }
        out = -pid->out_max;
    }
    return out;
}

void pid_f32_reset(pid_f32_t *pid);
void pid_f32_update_param(pid_f32_t *pid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PID_H */
