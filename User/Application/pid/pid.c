/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "pid.h"

/**
 * @brief Reset pid instance.
 *
 * @param pid PID instance
 */
void pid_f32_reset(pid_f32_t *pid)
{
    memset(pid->arm_pid.state, 0, sizeof(pid->arm_pid.state));
}

/**
 * @brief Update PID parameters, without reset.
 *
 * @param pid PID instance
 */
void pid_f32_update_param(pid_f32_t *pid)
{
    arm_pid_init_f32(&pid->arm_pid, 0);
}