# 基于STM32的电力机柜多参量状态监测系统

## 项目简介

针对电力机柜散热风机在无人值守场景下的安全监测需求，设计并实现了一套基于STM32F103C8T6的低成本多参量监测与预警系统。系统可实时监测温度、柜门倾斜状态、外力敲击三类典型异常事件，并实现分级声光报警与本地数据显示。

## 功能特性

- **温度监测**：KY-013 NTC热敏电阻采集温度，ADC转换，OLED实时显示
- **柜门状态检测**：KY-020滚珠倾斜开关，检测柜门非法开启
- **敲击检测**：KY-031敲击传感器，检测外力撞击
- **分级报警**：温度 > 倾斜 > 敲击三级优先级调度
- **人机交互**：0.96寸SSD1306 OLED（I2C接口），中文界面显示

## 技术栈

| 项目 | 内容 |
|------|------|
| 主控芯片 | STM32F103C8T6 |
| 开发环境 | Keil MDK-ARM |
| 编程语言 | C |
| 传感器 | KY-013（NTC）、KY-020（滚珠开关）、KY-031（敲击传感器） |
| 显示模块 | 0.96寸OLED（SSD1306，I2C） |
| 报警模块 | 蜂鸣器、LED |

## 报警优先级逻辑

| 优先级 | 触发条件 | 蜂鸣器 | LED | 说明 |
|:------:|----------|:------:|:---:|------|
| 高 | 温度 > 30℃ | 持续长鸣 | 常亮 | 超温报警，最高优先级 |
| 中 | 柜门打开（倾斜） | 不响 | 常亮 | 柜门开启提示 |
| 低 | 敲击（暴力闯入） | 短鸣0.5s | 同步0.5s | 瞬时响应 |

## 关键代码结构

```c
while(1) {
    temperature = KY013_GetTemperature();
    tilt_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
    knock_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
    
    if(temperature > 30.0f) {
        BEEP_ON(); LED_ON();
    }
    else if(tilt_state == 1) {
        BEEP_OFF(); LED_ON();
    }
    else {
        if(knock_state == 0 && !knock_active) {
            knock_active = 1;
            BEEP_ON(); LED_ON();
            delay_ms(500);
            BEEP_OFF(); LED_OFF();
            knock_active = 0;
        }
    }
    OLED_ShowTemperature(temperature);
    delay_ms(10);
}
