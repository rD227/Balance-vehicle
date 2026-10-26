# 命令行烧录、串口与右电机排查

适用目录：`/home/xvsu/MCUproject/Balance-vehicle`。不要在红外工程目录编译、烧录。以下操作由你手动执行；本次仅生成了固件，没有自动烧录或发送电机运行命令。

## 这次串口记录说明了什么

`/home/xvsu/Downloads/records-2026-10-23-20-54-09.json` 中有如下往返：

```text
发送 r
>> RUN RIGHT(BO1/BO2) motor: duty=25% (400ms)
>> STOP (pulse finished)
发送 R
>> RUN RIGHT(BO1/BO2) motor: duty=-25% (400ms)
>> STOP (pulse finished)
```

这证明 MCU 收到了指令，执行了电机 API 调用和停止分支；不证明 PWM 实际到达模块、电源正常或驱动桥完好。编码器没有接线，原记录的 `spd=0/s` 不能当作实测车轮速度，偶尔的计数也可能来自悬空输入。

当前应用保留默认 25% 占空比、400ms 点动和 1 秒最短间隔。已默认关闭上电自动正反转，增加 `d` 状态打印，并将编码器显示改为原始总计数和采样增量，不再标为速度。上电打印 `Boot: STBY=0, manual commands only.` 可用于确认新版本。

2026-10-23 21:11:15 的新记录显示：发送 `r` 后 CCR2=900，但 STBY=0、BIN1=0、BIN2=0。正向运行应为 1、1、0，因此异常的是 STBY 与 BIN1 读回低，不只是测量时间太短。新版另外打印 GPIOB 的时钟、ODR、IDR、CRL、CRH；若启动后控制脚读回与指令不一致，立即拉低 STBY 并报告 `GPIO readback mismatch; stopped`。

新版 `t/T` 是 5 秒右路正反向**空载测量命令**，须先从 BO1/BO2 拔掉电机。它沿用所选占空比，默认 25%，不自动改为满占空比。`r/R/l/L` 仍为 400ms，`x`/空格可提前停止；异常读回也会提前停止，不能保证异常状态下仍保持 5 秒。

## 编译与烧录

在 VSCode 菜单“终端 → 新建终端”中逐行输入，每行回车；某一步报错就先停止后续操作。烧录前断开 VM、电机；停止 VSCode 正在运行的调试会话，释放 ST-Link。

```bash
cd /home/xvsu/MCUproject/Balance-vehicle
cmake --preset Debug
cmake --build --preset Debug
arm-none-eabi-objcopy -O binary build/Debug/Balance-vehicle.elf build/Debug/Balance-vehicle.bin
st-flash --reset write build/Debug/Balance-vehicle.bin 0x08000000
```

前两条 CMake 命令分别配置构建、编译链接；objcopy 从 ELF 生成可烧录的 BIN；st-flash 将 BIN 写到 STM32 主 Flash 起始地址并复位。不要把 ELF 直接作为 binary 格式输入。[st-flash 官方说明](https://github.com/stlink-org/stlink/blob/testing/doc/man/st-flash.md)

烧录命令会改写固件。确认输出显示写入和校验成功，出现错误不要当作已烧录成功。运行时无需保持 GDB 启动；退出串口工具也不会自动停机。没有自动执行整片擦除或修改保护位的步骤。

## 查看串口

本机目前没有安装 picocom。你可执行一次：

```bash
sudo pacman -S picocom
```

先在浏览器点击断开串口，避免浏览器与终端争用设备。然后：

```bash
picocom -b 115200 -d 8 -y n -p 1 -f n /dev/ttyUSB0
```

这表示 115200、8 数据位、无校验、1 停止位、无流控。连接后直接按 `?` 就会发送一个字符，无需回车；默认没有本地回显，所以未必看得到你按的字符，但应收到 MCU 返回的帮助。按 `d` 查询右轮控制状态，不启动电机。按主控复位键可重新看启动文本。

退出：先按 **Ctrl+A**，松开，再按 **Ctrl+X**，这是两组组合键，不是同时按三个键。[picocom 官方手册](https://github.com/npat-efault/picocom/blob/master/picocom.1.md)

如果端口不是 ttyUSB0，先用 `ls /dev/serial/by-id/` 查看实际设备；也可以用完整稳定路径 `/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0` 替代 ttyUSB0。本机用户已在 uucp 组，不需要为了串口操作使用 sudo 或修改设备全局权限。

USB 串口 TX→PA3，RX→PA2，GND 共地，TX 输出应为 3.3V；主控正确单独供电时，串口电源脚不接。

## 占空比与 d 命令

`1`～`5` 只选择后续点动占空比 10%～50%；`r/R` 才会点动右轮正反向，`l/L` 控制左轮，`x` 或空格关闭两个桥。当前不建议通过提高占空比排查不转问题，先检查电源和控制信号；25% 也不能保证可启动或限制峰值电流。

若只为万用表测量输出，拔掉电机后使用 `t/T`（5000ms），不要用这两个命令延长带电机的堵转测试。

初始 25% 下，发送 `r` 后新程序会自动附加 MCU 状态；在 **VM 断开、电机拔掉** 时也能测试这些逻辑：

```text
>> MCU: STBY=1 BIN1=1 BIN2=0 PSC=0 ARR=3599 CCR2=900 CEN=1 CH2=1 MOE=1
```

`R` 时 BIN1/BIN2 应交换。400ms 后再按 `d`，应有 STBY=0、BIN1=0、BIN2=0、CCR2=0；CEN/CH2/MOE 可以仍为 1，因为 PWM 外设继续运行，桥由 STBY 禁用。

这些值来自 MCU GPIO 输入读回和 TIM1 寄存器，不是模块端的实测电平，也不是电机转动反馈。25% 对应 `CCR2=3600×0.25=900`；20kHz PWM 周期为 50µs。在 25% 时理想高电平宽度约 12.5µs，万用表无法验证这一波形。

## 只有万用表时怎样定位

先断电检查共地和接线：PB0→STBY，PB14→BIN1，PB15→BIN2，PA9→PWMB；右电机绕组两线接 BO1/BO2，不能把其中一线接 GND。未知的编码器引脚继续保持不接。

1. **VM 电源线从模块拔下，先量电源输出本身。** 记录来源、电压、额定电流；规划是 6V，不把实际值当作已确认。电阻/通断档只能在断电后使用。
2. **只接正确的逻辑电源和共地**，量模块 VCC—GND，应约 3.3V；静止时量模块 STBY—GND，应接近 0V。修正接线后不发热，仍不能证明桥输出已经正常。
3. 对照 `r` 附带的 MCU 状态。若 STBY、BIN1/BIN2、CCR2、使能位不符合上面预期，先查 MCU 控制；若符合，还需检查信号线确实连到模块对应端。万用表来不及捕捉 400ms 的电平，读到 0V 不能直接认定信号不存在。
4. 如需持续测量方向和 STBY，**VM 必须物理断开、电机拔掉**。在 VSCode 调试中，给 `StartPulse()` 内的 `started_at = HAL_GetTick();` 一行设断点，此时 Motor_Enable/SetSpeed 已执行。继续运行后发 `r`，暂停在该处，再量模块 STBY≈3.3V、BIN1≈3.3V、BIN2≈0V；逐根比较 MCU 引脚与模块引脚，可以查出断线或接错。PA9 的万用表直流平均值只能辅助判断；若定时器被调试器冻结，读数也会改变。
5. 测完删除断点、退出调试并复位，确认 STBY 恢复低，再考虑后续供电测试。**接着 VM 时不要用上述断点方法**：CPU 停住后，软件 400ms 停机不再执行，而定时器可能继续输出。

以上仍不足以确认驱动与电机电流匹配。待 VM/VCC、模块端控制电平和电流条件确认后，再做空载输出与单台电机测试；异常发热应立即切断 VM，不通过持续加大占空比试探。

## 接 VM 的空载 BO 输出测量

断 VM 是隔离功率回路、单独检查 MCU 控制电平的步骤，**测量 BO1/BO2 功率输出时必须有正确 VM 供电**。先拔掉电机，核对电源极性、逻辑电源和共地，再进行空载测试。

| TB6612 模块端 | 所需连接 |
| --- | --- |
| VCC | 3.3V 逻辑电源 |
| VM | 已确认的电机电源，本项目规划稳压 6V |
| GND / PGND | 电机电源负极及 STM32 GND，共地 |
| STBY | PB0，不接 5V，也不与电源硬短接 |
| BIN1 | PB14 |
| BIN2 | PB15 |
| PWMB | PA9 / TIM1_CH2 |
| BO1、BO2 | 本次只接万用表两表笔，不接电机 |

A 通道无需接电机来测试 B 通道。以上按模块丝印连接，不代表排针物理排列。万用表使用直流电压档（例如 20V 档），红表笔接 BO1，黑表笔接 BO2；不要使用电流档跨接这两个输出。发送 `t`，默认 25% 时，若 VM=6V、桥正常工作，理想时间平均差分电压约 +1.5V；`T` 极性相反。实际读数受模块压降、PWM、表的平均方式等影响，并不等于必须显示精确 1.50V。

还应同时确认 VM—GND 的实际供电电压、VCC—GND 约 3.3V，以及运行期间 STBY—GND 约 3.3V。STBY 低时输出是高阻待机，BO 差分读到 0V 或漂浮电压不能据此判断桥已损坏。[TB6612FNG 控制表](https://toshiba.semicon-storage.com/info/TB6612FNG_datasheet_en_20141001.pdf?did=10660&prodName=TB6612FNG)

新版 `d`/运行附带输出的解释：ODR 是输出锁存器，IDR 是引脚电平读回，CRL/CRH 是引脚模式，CLK 表示 GPIOB 时钟。对正向指令，ODR 中 PB0/PB14/PB15 对应位应为 1/1/0；若 ODR 正确但 IDR 不符，还要检查模式及外部短接、负载或引脚损坏。若 ODR 也不符，则先检查时钟、程序执行和固件。GPIOB 其他位可能有别的用途，不要求整个 ODR/IDR 恰好等于某个固定十六进制值。

## 本次验证

Debug 编译与 BIN 转换通过，FLASH 使用 18,864B/64KB、RAM 使用 2,408B/20KB（含链接预留堆栈）。主机 HAL 替身检查通过：无上电动作、d 查询、25% CCR2=900、反向、400ms 与 5 秒期限、忙时命令不续期、1 秒间隔、提前停止、GPIO 低电平故障停机、ODR/IDR 区分和 tick 回绕；不等同于实物电流或波形验证。本次未烧录；本次修改前备份在 `/tmp/balance-before-meter-test-hzrrx657/`，前一版本备份在 `/tmp/balance-before-cli-6b740ip9/`。
