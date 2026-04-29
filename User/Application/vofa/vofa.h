/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __VOFA_H
#define __VOFA_H

#include "foc_motor/foc_motor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void vofa_init(foc_motor_t *fm);
void vofa_foc_data_send(foc_motor_t *fm);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VOFA_H */
