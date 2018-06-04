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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_it.h"
#include "main.h"
#include "fonctions.h"
/** @addtogroup STM32F3-Discovery_Demo
  * @{
  */

/* Private define ---------------------------------------------------------*/
#define NB_MOY 64 // moyenne glissante 

/* Private variables ---------------------------------------------------------*/

// variables travail ADC moyennees de Acq_ADC1[6]
__IO uint32_t VAR_ADC12_IN6 , VAR_ADC12_IN7 , VAR_ADC12_IN8 , VAR_ADC12_IN9,VAR_ADC1_IN2,VAR_ADC1_IN5 ;

//variable pour surveillance de connexion avec raspberry pi par usart (detection de reception)
int watchdog;

// variable pour detection de même consigne de raspberry pi 
float old_cons;
int cmpt_egal = 0;

//	variable de l'état arrêt urgence declecnhé par Raspberry Pi 
int arret = 1;


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


/**
  * @brief  Interruption déclenché par Accelerometre à 94 HZ (Interruption Principale de système)
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{ 
	/** 
	 * Dectection de perde de connexion de USART
	 * La surveillance de connexion avec raspberry pi par usart (detection de reception)
	 * Si on ne reçoit pas de données de Raspberry Pi plus de 500ms, le LED clignote et le consigne_new =0
	 */
	watchdog++;
	if (watchdog >= 50){
		STM_EVAL_LEDToggle(LED8);
		watchdog = 0;
		Consigne1 = 0.0f;
		Consigne2 = 0.0f;
	}	
	
	// Lecture de donnée et remplir buffer envoyer, puis envoyer de donnée 
	Trait_Gyro_Acc ();
	
	// Sortir de boucle lorque l'envoie est fini
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
 
  EXTI_ClearITPendingBit(EXTI_Line1);
}


/**
  * @brief  Interruption par DMA de ADC, le boucle de courant est fait ici également 
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler (void)
{
	static uint16_t i = 0;	
	
	if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {	//test pas utile car seule
		i++;

		/* moyenne glissante NB_MOY passage*/
		VAR_ADC12_IN6 += Acq_ADC1[0];
		VAR_ADC12_IN7 += Acq_ADC1[1];
		VAR_ADC12_IN8 += Acq_ADC1[2];
		VAR_ADC12_IN9 += Acq_ADC1[3];
		VAR_ADC1_IN2  += Acq_ADC1[4];
		VAR_ADC1_IN5  += Acq_ADC1[5];

		if (i == NB_MOY)
		{
			i =0;
			MOY_ADC12_IN6 = VAR_ADC12_IN6/NB_MOY;
			MOY_ADC12_IN7 = VAR_ADC12_IN7/NB_MOY;
			MOY_ADC12_IN8 = VAR_ADC12_IN8/NB_MOY;
			MOY_ADC12_IN9 = VAR_ADC12_IN9/NB_MOY;
			MOY_ADC1_IN2  = VAR_ADC1_IN2/NB_MOY;
			MOY_ADC1_IN5  = VAR_ADC1_IN5/NB_MOY;

			VAR_ADC12_IN6 = 0;
			VAR_ADC12_IN7 = 0;
			VAR_ADC12_IN8 = 0;
			VAR_ADC12_IN9 = 0;
			VAR_ADC1_IN2  = 0;
			VAR_ADC1_IN5  = 0;
	    STM_EVAL_LEDToggle(LED9);
		
			if (ConfigOK==1){ 								//	une fois la configuration du système est fait
				Trait_b_courant();							//	execution boucle de courant une fois ADC a trouvé la valeur moyenne de courant
			}
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);			
	}	
}

/**
  * @brief  Interruption par reception USART de Raspberry Pi 
  * @param  None
  * @retval None
  */
void USART2_IRQHandler  (void)
{
  static int rx_index = 0;		// variable intern pour index de trame reçu
 	unsigned char tampon = 0; 	// variable temporaire pour tester le premier caractere de trame 
	float value;								// variable temporaire pour valeur de donnée de trame
	
	STM_EVAL_LEDToggle(LED4);

  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // Received characters modify string
  { 
		tampon = USART_ReceiveData(USART2);
		watchdog = 0;																					
		// Remis le watchdog à 0 car il y a encore communication de USART 
		
		/* Detection de trame complet de Raspberry Pi
				- On interprète le caratère de trame pour indentifier le debut de trame
				- Si c'est bien le début, on le prend et le stocker dans un buffer
		*/
		if (tampon == '<'){
			rx_index = 1;
			RX_USART[0] = '<';
		} else{
			RX_USART[rx_index++] = tampon;		
		}	
			
		// Interprete quand de buffer est rempli comme le taille de trame
    if (rx_index > (sizeof(RX_USART) - 1)){
			rx_index = 0;
			
			// Detection de trame complet 
			if (RX_USART[0] == '<'&& RX_USART[6] == '\n') 
			{
				value=bytes_to_float(&RX_USART[2]);	
				// interpretation de type de donnée dans le trame par label de trame
				switch (RX_USART[1]){
					//cas de reception de trame de consigne 
					case 'c' : 	Consigne_OK=1;
											Consigne_new=value/0.80435f;
											// detection si le consigne ancien est le même 
											// precaution de erreur de communication 
											if(old_cons == Consigne_new){
												cmpt_egal++;
											}
											else{
												cmpt_egal = 0;
												STM_EVAL_LEDOff(LED5);
											}
											old_cons = Consigne_new;
											STM_EVAL_LEDOn(LED3);
											cmpt_arr++;
										 break;
					//cas de reception de trame de arret urgence 
					case'a' : arret=1;
										cmpt_arr = 0;
										android = 0;
										break;
					//cas de reception d'angle de consigne						
					case 'd' : android=1;
										angleAndroid = value;	
										break;																		
					// des autres cas pas defini
					default : Consigne_OK= 0;	
										cmpt_arr++;
				}
			}
		}
	}			
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

