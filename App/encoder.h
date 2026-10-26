#ifndef APP_ENCODER_H
#define APP_ENCODER_H

#include "stm32f1xx_hal.h"
#include "motor.h"

/* Initialize and start hardware quadrature encoder timers:
 * - MOTOR_LEFT  (A-port): TIM2 CH1/CH2 on PA0/PA1
 * - MOTOR_RIGHT (B-port): TIM3 CH1/CH2 on PA6/PA7
 */
HAL_StatusTypeDef Encoder_Init(void);

/* Wheel-shaft counts per revolution (x4 quadrature), measured by hand-turning
 * the wheel: ~1000. Refine by turning 10 revolutions and dividing by 10.
 */
#define ENCODER_COUNTS_PER_REV  1000

/* Read signed count change since the last Encoder_GetDelta() call for this motor,
 * and accumulate into the 32-bit total position counter.
 */
int16_t Encoder_GetDelta(MotorId motor);

/* Return accumulated 32-bit total encoder count for this motor. */
int32_t Encoder_GetTotal(MotorId motor);

/* Return raw 16-bit hardware timer counter (TIM2->CNT or TIM3->CNT). */
uint16_t Encoder_GetRawCounter(MotorId motor);

/* Reset hardware counter and accumulated total for one motor (or both). */
void Encoder_Reset(MotorId motor);
void Encoder_ResetAll(void);

#endif /* APP_ENCODER_H */
