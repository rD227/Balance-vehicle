# Balance-vehicle 引脚配置

芯片：STM32F103C8T6，LQFP48。HSE 与 PLL 配置保留，SYSCLK 为 72 MHz；PA13/PA14 保留 Serial Wire。

| 用途 | 引脚 | 外设/模式 | 代码标签 |
| --- | --- | --- | --- |
| 左轮编码器 A | PA0 | TIM2_CH1 | LEFT_ENCODER_A |
| 左轮编码器 B | PA1 | TIM2_CH2 | LEFT_ENCODER_B |
| 右轮编码器 A | PA6 | TIM3_CH1 | RIGHT_ENCODER_A |
| 右轮编码器 B | PA7 | TIM3_CH2 | RIGHT_ENCODER_B |
| 左轮 PWM | PA8 | TIM1_CH1 | LEFT_PWM |
| 右轮 PWM | PA9 | TIM1_CH2 | RIGHT_PWM |
| 左电机 AIN1 | PB12 | GPIO 推挽输出 | LEFT_AIN1 |
| 左电机 AIN2 | PB13 | GPIO 推挽输出 | LEFT_AIN2 |
| 右电机 BIN1 | PB14 | GPIO 推挽输出 | RIGHT_BIN1 |
| 右电机 BIN2 | PB15 | GPIO 推挽输出 | RIGHT_BIN2 |
| TB6612 STBY | PB0 | GPIO 推挽输出 | TB6612_STBY |
| MPU6050 SCL | PB6 | I2C1_SCL，复用开漏 | MPU6050_SCL |
| MPU6050 SDA | PB7 | I2C1_SDA，复用开漏 | MPU6050_SDA |
| 调试串口 TX | PA2 | USART2_TX | DEBUG_TX |
| 调试串口 RX | PA3 | USART2_RX | DEBUG_RX |

## 初始参数

- TIM2/TIM3：Encoder TI1+TI2 模式，四倍频计数；PSC=0，ARR=65535，无输入滤波、无内部上下拉。
- TIM1：PWM mode 1，高电平有效；PSC=0，ARR=3599，72 MHz / 3600 = 20 kHz；CH1/CH2 初始 CCR=0。
- I2C1：100 kHz，7 位地址模式。
- USART2：115200 baud，8 数据位、1 停止位、无校验、无硬件流控。
- 电机方向脚和 STBY：初始化输出低电平。

初始化代码由 CubeMX 生成在 `Core/Src/tim.c`、`i2c.c`、`usart.c`、`gpio.c`。引脚宏位于 `Core/Inc/main.h`。

第一阶段应用位于 `App/motor.c`、`App/motor_test.c`：调用 `HAL_TIM_PWM_Start()` 启动 TIM1 CH1/CH2，上电保持 STBY 低；只有手动串口点动命令才使能驱动。接线、供电检查及测试方法见 [MOTOR_TEST.md](MOTOR_TEST.md)。

编码器尚未调用 `HAL_TIM_Encoder_Start()`。MPU6050 驱动及 PID 尚未添加，未做实物功能验证。

## 命令行重新生成与构建

先将 CubeMX GUI 中的编辑保存并关闭该项目，再修改 `.ioc`，然后在项目根目录执行：

```bash
bash tools/generate.sh
cmake --preset Debug
cmake --build --preset Debug
```

`tools/generate.sh` 会读取当前项目的 `.ioc` 并调用 CubeMX `-q` 模式重新生成工程；它不负责修改引脚。AI 可以修改 `.ioc` 后执行此脚本，省去 GUI 中逐个点击。

默认使用 PATH 中的 `stm32cubemx`；可通过 `STM32CUBEMX=/path/to/executable` 指定可执行文件。本机 CubeMX 6.18.1 即使使用 `-q` 也需要可用的图形会话。

修改应用代码时使用 `USER CODE BEGIN/END` 区块或独立文件，便于后续重新生成。
