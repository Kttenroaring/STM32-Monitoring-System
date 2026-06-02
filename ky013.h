#ifndef __KY013_H
#define __KY013_H

#include "stm32f10x.h"

void KY013_Init(void);
float KY013_GetTemperature(void);

#endif