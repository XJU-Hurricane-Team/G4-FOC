/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __MC_CONFIG_H
#define __MC_CONFIG_H

/******************************************************************************
 * @defgroup TIMER configuration, refer to MOS datasheet.
 * @{
 */

#define MCPWM_CFG_TDON_DELAY_NS  6  /* MOS drain on delay time (ns) */
#define MCPWM_CFG_TDOFF_DELAY_NS 20 /* MOS drain off delay time (ns) */
#define MCPWM_CFG_TRISE_NS       9  /* MOS rise time (ns) */
#define MCPWM_CFG_TFALL_NS       3  /* MOS fall time (ns) */

#define MCPWM_DEADTIME_PERIODS   ()

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MC_CONFIG_H */
