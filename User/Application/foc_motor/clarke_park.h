/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __CLARKE_PARK_H
#define __CLARKE_PARK_H

#include <arm_math.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif /* !__STATIC_FORCEINLINE */

/**
 * @brief Clarke Transform - Three-phase to Two-phase
 *
 * Converts three-phase currents (Ia, Ib, Ic) into (Iα, Iβ) in the stationary coordinate system.
 *
 * Transformation formula:
 *   Iα = Ia
 *   Iβ = (Ib - Ic) / √3
 *
 * Description:
 *   - Used as the first transformation step after current sampling.
 *   - Ia is the phase A current, while Ib and Ic are sampled through other channels.
 *   - The output Iα and Iβ are components in the stationary coordinate system.
 *
 * @param m instance of motor parameter
 *  @args[in]  m->Ia        Phase A current
 *  @args[in]  m->Ib        Phase B current
 *  @args[in]  m->Ic        Phase C current
 *  @args[out] m->I_alpha   Alpha-axis current
 *  @args[out] m->I_beta    Beta-axis current
 */
__STATIC_FORCEINLINE void clarke_transform(motor_iv_param_t *m)
{
    m->I_alpha = m->Ia;
    m->I_beta = (m->Ib - m->Ic) * 0.57735026919f; /* 1/√3 = 0.57735026919 */
}

/**
 * @brief Inverse Clarke Transform - Two-phase to Three-phase
 *
 * Converts voltages (Vα, Vβ) in the stationary coordinate system back to
 * three-phase voltages (Va, Vb, Vc). This is used before SVPWM modulation.
 *
 * Transformation formula:
 *   Va = Vα
 *   Vb = -0.5*Vα + (√3/2)*Vβ
 *   Vc = -0.5*Vα - (√3/2)*Vβ
 *
 * Description:
 *   - This is the inverse of the Clarke transform.
 *   - Used after the inverse Park transform from the dq coordinate system to
 *     obtain three-phase voltages.
 *   - Subsequently input into the SVPWM modulator.
 *
 * @param m instance of motor parameter
 *  @args[in]  m->V_alpha   Alpha-axis voltage
 *  @args[in]  m->V_beta    Beta-axis voltage
 *  @args[out] m->Va        Phase A voltage
 *  @args[out] m->Vb        Phase B voltage
 *  @args[out] m->Vc        Phase C voltage
 */
__STATIC_FORCEINLINE void clarke_inverse_transform(motor_iv_param_t *m)
{
    m->Va = m->V_alpha;
    m->Vb = -0.5f * m->V_alpha + 0.86602540378f * m->V_beta; /* √3/2 = 0.86602540378 */
    m->Vc = -0.5f * m->V_alpha - 0.86602540378f * m->V_beta;
}

/**
 * @brief Park Transform - Alpha-Beta to dq
 *
 * Converts currents (Iα, Iβ) in the stationary coordinate system to (Id, Iq)
 * in the rotating coordinate system. Requires the rotor electrical angle theta.
 *
 * Transformation formula:
 *   Id = Iα*cos(θ) + Iβ*sin(θ)      // d-axis is the direct axis (flux axis)
 *   Iq = Iβ*cos(θ) - Iα*sin(θ)      // q-axis is the quadrature axis (torque axis)
 *
 * Description:
 *   - θ is the motor rotor electrical angle (obtained from an encoder).
 *   - cos(θ) and sin(θ) need to be pre-calculated and stored.
 *   - The Id component is aligned with the flux direction (often set to 0 for MTPA control).
 *   - The Iq component controls motor torque and is proportional to it.
 *   - One of the core transformations of the FOC algorithm.
 *
 * @param m instance of motor parameter
 *  @args[in]  m->I_alpha   Alpha-axis current
 *  @args[in]  m->I_beta    Beta-axis current
 *  @args[in]  m->sin_val   Value of sin(θ), where θ is the rotor electrical angle
 *  @args[in]  m->cos_val   Value of cos(θ)
 *  @args[out] m->Id        d-axis current
 *  @args[out] m->Iq        q-axis current
 */
__STATIC_FORCEINLINE void park_transform(motor_iv_param_t *m)
{
    m->Id = m->I_alpha * m->cos_val + m->I_beta * m->sin_val;
    m->Iq = m->I_beta * m->cos_val - m->I_alpha * m->sin_val;
}

/**
 * @brief Inverse Park Transform - dq to Alpha-Beta
 *
 * Converts voltages (Vd, Vq) in the rotating coordinate system back to (Vα, Vβ)
 * in the stationary coordinate system. This is the inverse process of the Park transform.
 *
 * Transformation formula:
 *   Vα = Vd*cos(θ) - Vq*sin(θ)
 *   Vβ = Vd*sin(θ) + Vq*cos(θ)
 *
 * Description:
 *   - Used to transform PID controller outputs in the dq coordinate system back
 *     to the Alpha-Beta coordinate system.
 *   - The transformed voltages will be input into the inverse Clarke transform,
 *     and finally modulated into PWM via SVPWM.
 *   - θ must be consistent with the electrical angle used in the corresponding Park transform.
 *
 * @param m instance of motor parameter
 *  @args[in]  m->Vd        d-axis voltage (direct axis, excitation voltage)
 *  @args[in]  m->Vq        q-axis voltage (quadrature axis, torque voltage)
 *  @args[in]  m->sin_val   Value of sin(θ)
 *  @args[in]  m->cos_val   Value of cos(θ)
 *  @args[out] m->V_alpha   Alpha-axis voltage
 *  @args[out] m->V_beta    Beta-axis voltage
 */
__STATIC_FORCEINLINE void park_inverse_transform(motor_iv_param_t *m)
{
    m->V_alpha = m->Vd * m->cos_val - m->Vq * m->sin_val;
    m->V_beta = m->Vd * m->sin_val + m->Vq * m->cos_val;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CLARKE_PARK_H */
