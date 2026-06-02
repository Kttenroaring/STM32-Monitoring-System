#include "stm32f10x.h"
#include "OLED.h"
#include "ky013.h"
#include "FMQ.h"
#include "Delay.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}

void Tilt_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Knock_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main(void)
{
    float temperature;
    uint8_t tilt_state;
    uint8_t knock_state;

    uint8_t knock_active = 0;
    uint32_t knock_timer = 0;

    OLED_Init();
    KY013_Init();
    mfq_Init();
    LED_Init();
    Tilt_Init();
    Knock_Init();

    OLED_ShowChinese(0, 0, "温度：");
    OLED_ShowChinese(94, 0, "℃");
    OLED_Update();

    while(1)
    {
        temperature = KY013_GetTemperature();
        tilt_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
        knock_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);

        OLED_ShowNum(47, 0, (uint32_t)temperature, 2, OLED_8X16);
        OLED_Update();

        // 温度报警（最高优先级）
        if(temperature > 30.0f)
        {
            knock_active = 0;
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
        }
        // 倾斜报警
        else if(tilt_state == 1)
        {
            knock_active = 0;
            GPIO_ResetBits(GPIOB, GPIO_Pin_0);
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);  // LED亮
        }
        else
        {
            // 敲击检测
            if(knock_state == 0 && knock_active == 0)
            {
                knock_active = 1;
                knock_timer = 0;
            }

            if(knock_active)
            {
                knock_timer++;
                if(knock_timer < 500)   // 500 * 1ms = 500ms？不对，后面Delay_ms(1)循环，实际为500ms
                {
                    GPIO_SetBits(GPIOB, GPIO_Pin_0);
                    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
                }
                else
                {
                    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
                    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
                    knock_active = 0;
                    knock_timer = 0;
                }
            }
            else
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_0);
                GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
            }
        }

        Delay_ms(10);   // 1ms循环，利于检测敲击
    }
}