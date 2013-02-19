#ifndef __ACC_H
#define __ACC_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stdio.h"
#include "stm32f4_discovery_lis302dl.h"

void initAcc(void);
void initAccClickInterrupt(void);
void clickConfig(void);
	
#endif // __ACC_H
