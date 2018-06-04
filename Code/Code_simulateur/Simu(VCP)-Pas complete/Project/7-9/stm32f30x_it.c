/**
  ******************************************************************************
  * @file    stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_it.h"
#include "main.h"
//#include "usb_istr.h"
#include "fonctions.h"
/** @addtogroup STM32F3-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t UserButtonPressed;
__IO uint32_t VAR_ADC12_IN8 , VAR_ADC12_IN9 ;// variables travail ADC moyennees de Acq_ADC1[2]

int watchdog;
int lost_connexion = 0;

#define NB_MOY 64 // moyenne glissante 
__IO uint32_t i =0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                               */
/******************************************************************************/



void EXTI0_IRQHandler(void)
{ 
  if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
		if (usage_acc ==1)
    {
			usage_acc=0;
    }else{
			usage_acc=1;
		}
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}


// INT2 GYRO PE1
void EXTI1_IRQHandler(void)
{ 
  STM_EVAL_LEDOn(LED10);	
	watchdog++;
	if (watchdog >= 50){
		STM_EVAL_LEDToggle(LED8);
		lost_connexion = 1;
		watchdog = 0;
		Consigne = 0.0;
	}	
	
	Trait_Gyro_Acc ();
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
 	
  EXTI_ClearITPendingBit(EXTI_Line1);
	STM_EVAL_LEDOff(LED10);//Test fin de calcul
}

void DMA1_Channel1_IRQHandler (void)
{
	static uint16_t i = 0;	
	if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {
		i++;
		VAR_ADC12_IN8 += Acq_ADC1[0];
		VAR_ADC12_IN9 += Acq_ADC1[1];

		if (i == NB_MOY)
		{
			i =0;
			MOY_ADC12_IN8 = VAR_ADC12_IN8/NB_MOY;
			MOY_ADC12_IN9 = VAR_ADC12_IN9/NB_MOY;
			VAR_ADC12_IN8 = 0;
			VAR_ADC12_IN9 = 0;			
	    STM_EVAL_LEDToggle(LED9);
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);			
	}	
}

void USART2_IRQHandler  (void)
{
  static int rx_index = 0;
 	unsigned char tampon = 0; 	// variable temporaire pour tester le premier caractere de trame 
	float value;
	
	STM_EVAL_LEDToggle(LED4);

  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // Received characters modify string
  { 
		tampon = USART_ReceiveData(USART2);
		watchdog = 0;
		//STM_EVAL_LEDOff(LED3);
		if (tampon == '<'){
			rx_index = 1;
			RX_USART[0] = '<';
		} else{
			RX_USART[rx_index++] = tampon;		
		}	
			
    if (rx_index > (sizeof(RX_USART) - 1)){
			rx_index = 0;
			
			if (RX_USART[0] == '<'&& RX_USART[6] == '\n') // trame complet
			{
				value=bytes_to_float(&RX_USART[2]);	
				switch (RX_USART[1]){
					
						case 'c' : Consigne=value;
											 STM_EVAL_LEDToggle(LED3);
											 break;
						
						case'a' : if(value){
												arret = 1;
											}
											else{

												arret = 0;
											}
											break;
		
				}
			}
		}
	}			
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

