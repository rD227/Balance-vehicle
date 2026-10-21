#ifndef APP_MOTOR_H
#define APP_MOTOR_H

#include "stm32f1xx_hal.h"

typedef enum {
    MOTOR_LEFT = 0,
    MOTOR_RIGHT = 1
} MotorId;

/* Call after MX_GPIO_Init() and MX_TIM1_Init(). Starts PWM, leaves STBY low. */
HAL_StatusTypeDef Motor_Init(void);
/* Both channels coast before STBY is raised. No command is restored. */
HAL_StatusTypeDef Motor_Enable(void);
/* Signed PWM duty in per mille [-1000, 1000], NOT an RPM command.
 * Zero coasts; sign selects electrical polarity, not vehicle forward.
 * Call from the main loop only. Stop and allow the wheel to rest before reversal.
 */
HAL_StatusTypeDef Motor_SetSpeed(MotorId motor, int16_t duty_permille);
/* Shared STBY low: immediately disables BOTH bridges (coast, not active brake).
 * Requires Motor_Enable() before another movement command.
 * Software stop requires a running CPU; remove VM for a physical emergency stop.
 */
void Motor_EmergencyStop(void);

#endif
