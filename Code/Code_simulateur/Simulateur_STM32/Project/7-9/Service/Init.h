/**
  ******************************************************************************
  * @file    Init.h
  * @author  INSA Toulouse
  * @version V1.0
  * @date    13-Juin-2017
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_H
#define __INIT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"

/* Private define ------------------------------------------------------------*/
#define Freq_SYS 72000000							// 	Frequence systeme

/* Private structure ---------------------------------------------------------*/
//structure pour initialisation et configuration de peripheriques
extern RCC_ClocksTypeDef RCC_Clocks;
extern GPIO_InitTypeDef  GPIO_InitStructure;	
extern USART_InitTypeDef USART_InitStructure;
extern ADC_InitTypeDef       ADC_InitStructure;
extern ADC_CommonInitTypeDef ADC_CommonInitStructure;
extern DMA_InitTypeDef DMA_InitStructure;
	
/* Private variable ---------------------------------------------------------*/

//Variable pour ADC 
extern __IO uint16_t Acq_ADC1[2];					//	tableau de valeurs de lectures ADC
extern __IO uint16_t calibration_value;		//	valeur pour calibration de ADC

//Variable pour USART
extern uint8_t TX_USART [37];							//	Buffer emission
extern uint8_t RX_USART [7];							//	Buffer reception

/* Exported functions ------------------------------------------------------- */
void Init_IT_EXT(void);
void Init_IT_DMA1(void);
void Init_ADC1_DMA1(void);
void Init_USART(void);
void Init_Bouton(void);

#endif /* __INIT_H */

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/
