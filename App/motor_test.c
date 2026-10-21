#include "motor_test.h"
#include "motor.h"
#include "usart.h"

#define TEST_PULSE_MS       300U
#define TEST_REST_MS       2000U

static uint16_t test_duty = 100U; /* 10%; this is NOT current limiting. */
static uint8_t running;
static uint32_t started_at;
static uint32_t stopped_at;

static void PrintHelp(void)
{
    static const char text[] =
        "\r\nMotor test: STBY on boot. 115200 8N1.\r\n"
        "l/L: left +/-; r/R: right +/-. One 300ms pulse.\r\n"
        "1/2/3: duty 10/20/30% (idle only); x or space: stop BOTH.\r\n"
        "?: help (idle only). Wait >=2s AND wheel stopped before next pulse.\r\n"
        "Check wiring/current first. Low duty does NOT limit peak current.\r\n";
    /* Only used while STBY is low, never during a motor pulse. */
    (void)HAL_UART_Transmit(&huart2, (const uint8_t *)text,
                            (uint16_t)(sizeof(text) - 1U), 100U);
}

void MotorTest_Init(void)
{
    running = 0U;
    test_duty = 100U;
    stopped_at = HAL_GetTick();
    Motor_EmergencyStop();
    PrintHelp();
}

void MotorTest_Poll(void)
{
    uint8_t command;
    MotorId motor;
    int16_t duty;
    uint32_t now = HAL_GetTick();

    /* Check deadline before reading UART; more commands cannot extend it.
     * Unsigned subtraction also works across the HAL tick wraparound.
     */
    if (running != 0U && (uint32_t)(now - started_at) >= TEST_PULSE_MS) {
        Motor_EmergencyStop();
        running = 0U;
        stopped_at = now;
    }

    /* Timeout=0: sample at most one received byte without waiting for input. */
    if (HAL_UART_Receive(&huart2, &command, 1U, 0U) != HAL_OK) {
        return;
    }
    if (command == 'x' || command == ' ') {
        Motor_EmergencyStop();
        running = 0U;
        stopped_at = now;
        return;
    }
    if (running != 0U) {
        return;
    }
    if (command == '?') {
        PrintHelp();
        return;
    }
    if (command >= '1' && command <= '3') {
        test_duty = (uint16_t)(command - '0') * 100U;
        return;
    }
    if ((uint32_t)(now - stopped_at) < TEST_REST_MS) {
        return;
    }
    switch (command) {
    case 'l': motor = MOTOR_LEFT;  duty = (int16_t)test_duty; break;
    case 'L': motor = MOTOR_LEFT;  duty = -(int16_t)test_duty; break;
    case 'r': motor = MOTOR_RIGHT; duty = (int16_t)test_duty; break;
    case 'R': motor = MOTOR_RIGHT; duty = -(int16_t)test_duty; break;
    default: return; /* CR/LF and unknown bytes do not command motion. */
    }
    if (Motor_Enable() != HAL_OK || Motor_SetSpeed(motor, duty) != HAL_OK) {
        Motor_EmergencyStop();
        stopped_at = now;
        return;
    }
    started_at = HAL_GetTick();
    running = 1U;
}
