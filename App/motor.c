#include "motor.h"
#include "main.h"
#include "tim.h"

static uint8_t pwm_ready;
static uint8_t enabled;

static void Coast(MotorId motor)
{
    /* Datasheet Stop mode: IN1=IN2=0, PWM=1, STBY=1 -> high impedance.
     * PWM=0 alone is NOT a coast command (it brakes in drive mode).
     */
    if (motor == MOTOR_LEFT) {
        HAL_GPIO_WritePin(LEFT_AIN1_GPIO_Port,
                          LEFT_AIN1_Pin | LEFT_AIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
                              __HAL_TIM_GET_AUTORELOAD(&htim1) + 1U);
    } else {
        HAL_GPIO_WritePin(RIGHT_BIN1_GPIO_Port,
                          RIGHT_BIN1_Pin | RIGHT_BIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,
                              __HAL_TIM_GET_AUTORELOAD(&htim1) + 1U);
    }
}

void Motor_EmergencyStop(void)
{
    /* Drop STBY first: independent of CCR preload and PWM period. */
    HAL_GPIO_WritePin(TB6612_STBY_GPIO_Port, TB6612_STBY_Pin, GPIO_PIN_RESET);
    enabled = 0U;
    if (pwm_ready != 0U) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0U);
    }
    HAL_GPIO_WritePin(LEFT_AIN1_GPIO_Port,
                      LEFT_AIN1_Pin | LEFT_AIN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_BIN1_GPIO_Port,
                      RIGHT_BIN1_Pin | RIGHT_BIN2_Pin, GPIO_PIN_RESET);
}

HAL_StatusTypeDef Motor_Init(void)
{
    Motor_EmergencyStop();
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0U);
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {
        return HAL_ERROR;
    }
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        (void)HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
        return HAL_ERROR;
    }
    pwm_ready = 1U;
    return HAL_OK;
}

HAL_StatusTypeDef Motor_Enable(void)
{
    if (pwm_ready == 0U) {
        return HAL_ERROR;
    }
    if (enabled == 0U) {
        Coast(MOTOR_LEFT);
        Coast(MOTOR_RIGHT);
        /* Latch both preloaded CCRs while the bridges are still disabled. */
        if (HAL_TIM_GenerateEvent(&htim1, TIM_EVENTSOURCE_UPDATE) != HAL_OK) {
            return HAL_ERROR;
        }
        HAL_GPIO_WritePin(TB6612_STBY_GPIO_Port, TB6612_STBY_Pin, GPIO_PIN_SET);
        enabled = 1U;
    }
    return HAL_OK;
}

HAL_StatusTypeDef Motor_SetSpeed(MotorId motor, int16_t duty_permille)
{
    int32_t duty = duty_permille;
    uint32_t channel;
    uint32_t compare;
    uint16_t pin;
    GPIO_TypeDef *port;

    if ((motor != MOTOR_LEFT && motor != MOTOR_RIGHT) || enabled == 0U) {
        return HAL_ERROR;
    }
    if (duty > 1000) duty = 1000;
    if (duty < -1000) duty = -1000;
    if (duty == 0) {
        Coast(motor);
        return HAL_OK;
    }

    channel = motor == MOTOR_LEFT ? TIM_CHANNEL_1 : TIM_CHANNEL_2;
    port = motor == MOTOR_LEFT ? LEFT_AIN1_GPIO_Port : RIGHT_BIN1_GPIO_Port;
    HAL_GPIO_WritePin(port,
                      motor == MOTOR_LEFT ? LEFT_AIN1_Pin | LEFT_AIN2_Pin
                                          : RIGHT_BIN1_Pin | RIGHT_BIN2_Pin,
                      GPIO_PIN_RESET);
    if (motor == MOTOR_LEFT) {
        pin = duty > 0 ? LEFT_AIN1_Pin : LEFT_AIN2_Pin;
    } else {
        pin = duty > 0 ? RIGHT_BIN1_Pin : RIGHT_BIN2_Pin;
    }
    compare = ((uint32_t)(duty < 0 ? -duty : duty) *
               (__HAL_TIM_GET_AUTORELOAD(&htim1) + 1U)) / 1000U;
    __HAL_TIM_SET_COMPARE(&htim1, channel, compare);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    return HAL_OK;
}
