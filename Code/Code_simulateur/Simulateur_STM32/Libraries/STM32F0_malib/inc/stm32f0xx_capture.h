/**
  ******************************************************************************
  * @file    stm32f0xx_capture.h
  * @author   
  * @version
  * @date    mai-2013
  * @brief   
  *          
  *            
  *            
  *            
  *           
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0XX_capture_H
#define __STM32F0XX_capture_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_tim.h"
 

  

/* Private function prototypes -----------------------------------------------*/

void TIM_Config(void);
void init_cap_tim2(void);
void itcapt(void);
uint16_t mes_rpm (void);

#endif /* __STM32F0XX_capture_H */
/*
	**************************************************************************************
*/
