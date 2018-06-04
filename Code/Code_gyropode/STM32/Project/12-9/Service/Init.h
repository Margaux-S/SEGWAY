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
#define SHDNM1 GPIO_Pin_6 								//	Shutdown moteur 1 PC6
#define SHDNM2 GPIO_Pin_7 								//	Shutdown moteur 1 PC7
#define CMD_ALIM GPIO_Pin_8								//	Commande alim générale
#define facteur_prescaler 1 							// 	Facteur pour TIMER 15
#define Freq_PWM 25000										// 	Frequence PWM en Hz  
#define Freq_SYS 72000000									// 	Frequence systeme
#define PWM50  (Freq_SYS/(2*Freq_PWM))		// 	Initielisation compteur Timer pour 50 %

/* Private structure ---------------------------------------------------------*/
//structure pour initialisation et configuration de peripheriques
extern RCC_ClocksTypeDef RCC_Clocks;
extern GPIO_InitTypeDef  GPIO_InitStructure;	
extern USART_InitTypeDef USART_InitStructure;
extern ADC_InitTypeDef       ADC_InitStructure;
extern ADC_CommonInitTypeDef ADC_CommonInitStructure;
extern DMA_InitTypeDef DMA_InitStructure;
extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
extern TIM_OCInitTypeDef  TIM_OCInitStructure;
	
/* Private variable ---------------------------------------------------------*/

//Variable pour ADC 
extern __IO uint16_t Acq_ADC1[6];					//	tableau de valeurs de lectures ADC
extern __IO uint16_t calibration_value;		//	valeur pour calibration de ADC

//Variable pour USART
extern uint8_t TX_USART [37];							//	Buffer emission
extern uint8_t RX_USART [7];							//	Buffer reception

//Variable pour Timer Et PWM
extern uint16_t TimerPeriod; 							//	Variable pour de periode pour configuration de timer PWM	
extern uint16_t PWM_MOTEUR1;							//	Variable pour modifier de CCR1 de 16 bits de TIM pour moteur 1 par fonction TIM_SetCompare1
extern uint16_t PWM_MOTEUR2; 							//	Variable pour modifier de CCR1 de 16 bits de TIM pour moteur 2 par fonction TIM_SetCompare2

/* Exported functions ------------------------------------------------------- */
void Init_shutdown(void);
void Init_IT_EXT(void);
void Init_IT_DMA1(void);
void Init_ADC1_DMA1(void);
void Init_USART(void);
void TIM_Config(void);
void Init_Bouton(void);


#endif /* __INIT_H */

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/

