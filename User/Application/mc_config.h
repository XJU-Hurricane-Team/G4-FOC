/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __MC_CONFIG_H
#define __MC_CONFIG_H

/******************************************************************************
 * @defgroup TIMER configuration, refer to MOS and gate driver datasheet.
 * @refer https://community.infineon.com/t5/Knowledge-Base-Articles/Dead-time-calculation-for-Insulated-Gate-Bipolar-Transistors-IGBT/ta-p/434308
 *        https://community.infineon.com/t5/%E7%9F%A5%E8%AF%86%E5%BA%93%E6%96%87%E7%AB%A0/%E6%AD%BB%E5%8C%BA%E6%97%B6%E9%97%B4%E8%AE%A1%E7%AE%97%E5%85%AC%E5%BC%8F/ta-p/826663
 * @{
 */

#define MCPWM_CFG_DEADTIME_NS       (650U)       /* deadtime (ns) */
#define MCPWM_CFG_PWM_FREQ          (20 * 1000U) /* MOS PWM frequency (Hz) */
#define MCPWM_MAX_DUTY              (95)         /* MOS PWM Maximum duty (%) */
#define MCPWM_CFG_TGRO_OFFSET       (100U)       /* Offset of ADC sample trigger */
#define MCPWM_CFG_RCR               (1U)         /* Repetition Counter */

/**
 * @}
 */

/******************************************************************************
 * @defgroup Temperature, voltage configuration.
 * @note The actual over/under voltage will be multiplied by the number of cells.
 * @{
 */

/* temperature (℃) threshold */
#define MC_CFG_TEMP_LIMIT           (70.0f)
/* single battery cell over voltage threshold (V) */
#define MC_CFG_BAT_CELL_OVER_LIMIT  (4.2f)
/* single battery cell under voltage threshold (V) */
#define MC_CFG_BAT_CELL_UNDER_LIMIT (3.3f)
/* Maximum voltage limit (V) */
#define MC_CFG_MAX_VOLTAGE          (30.0f)
/* Maximum speed limit (rad/s) */
#define MC_CFG_MAX_SPEED            (25.12f)

/**
 * @}
 */

/******************************************************************************
 * @defgroup Motor parameters.
 * @{
 */

typedef struct {
    float rds;
} motor_param_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MC_CONFIG_H */
