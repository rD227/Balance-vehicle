#ifndef APP_MOTOR_TEST_H
#define APP_MOTOR_TEST_H

/* Stage 1 manual UART test. No encoder, PID or automatic motion sequence. */
void MotorTest_Init(void);
void MotorTest_Poll(void);

#endif
