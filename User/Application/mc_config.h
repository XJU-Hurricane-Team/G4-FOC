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
 * @defgroup Hardware configuration
 * @{
 */

/** Shunt sample resistor value (mΩ), should consider PCB resistor. */
#define MCHW_SHUNT_RESISTOR         (20.0f)
#define MCHW_OPAMP_GAIN             (50.0f) /** Operational amplifier gain */
#define MCHW_VBUS_ADC_R_UP          (10.0f) /** Power in voltage ADC pull up resistor (kΩ) */
#define MCHW_VBUS_ADC_R_DOWN        (1.0f)  /** Power in voltage ADC pull down resistor (kΩ) */
#define MCHW_NTC_ADC_R_UP           (10.0f) /** NTC ADC pull up resistor (kΩ) */

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
/* Debug data send period (ms). Set 0 to disable */
#define MC_CFG_VOFA_SEND_PERIOD     (10)
/**
 * @}
 */

/******************************************************************************
 * @defgroup calibration configuration.
 * @{
 */

/* Maximum current (A) when calibration. If the motor has difficulty rotating
 * or stops halfway through rotation during calibration, increase this value. */
#define MC_CFG_CALI_MAX_CURRENT     (0.5f)
/* Maximum voltage (V) when calibration. If the motor does not make a "beep"
 * sound during calibration, increase this value. */
#define MC_CFG_CALI_MAX_VOLTAGE     (3.0f)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MC_CONFIG_H */
