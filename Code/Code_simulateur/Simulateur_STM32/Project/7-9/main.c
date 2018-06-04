/**
  ******************************************************************************
  * @file    main.c  
  * @author  INSA Toulouse
  * @version V2.0 
  * @date    08 Septembre 2017 
  * @brief   Programme Principale de Simulateur
  *******************************************************************************/
	
 /* Les informations :
  *Code de simulateur
 	*PC2 ADC12_IN8 Potentiometre pour angle user
	*PC3 ADC12_IN9 tension de batterie 
	*
  *USART2 :	
  *PA3 USART2 RX
  *PA2 USART2 TX  
	*
	*PA4 Boutons poussoirs du guidon 
*/

/*
 *Indication de LED
 * LED 3 = clignoter quand la reception de trame de consgine 
 * LED 4 = Choix de entree de Beta => ON : Accelerometre OFF: Potentiometre 
 * LED 6 = ON quand le switch de presence user est fermé 

 * LED 8 = ON quand arret=1
 * LED 10 =
 * LED 5 = clignoter quand la reception de donnees via USART de RPI
 * LED 7 = clignoter quand pas de connexion de USART avec RPI
 * LED 9 = clignoter quand envoie de donnees via USART à RPI
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Variable global------------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */
int main(void)
{  
	// Configuration de horloge du systeme 
	RCC_GetClocksFreq(&RCC_Clocks);  											// 	Retourne la fréquence systeme 
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100000);		// 	Configuration du systick Tps = 1 / nbre
	NVIC_SetPriority(SysTick_IRQn, 3);										// 	Configuration priorité faible pour le Systick
	
	// Initialisation LEDs de STM32F3-Discovery board 
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);
	
	// Initialisation et configuration de ADC-DMA
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div2);								// 	ADC1 et ADC2 de Div1 à Div256
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);		//	ADC1 et ADC2 clock en commun
  Init_ADC1_DMA1();																			//	Configuration ADC1 
  Init_IT_DMA1();																				//	Configuration d'interruption de DMA pour ADC1 
  NVIC_SetPriority(DMA1_Channel1_IRQn, 1);							//  Configuration priorité 1 pour DMA
	
	// Initialisation et configuration de USART 
	Init_USART();																					//	Initialisation USART2
 
 	// Initialisation et configuration de acceletometre et gyroscope
	Acc_config(); 																				//	Initialisation LSM303DLHC  Accelerometr
  Gyro_config();																				// 	Initialisation Gyroscope L3GD20 par default avec L3GD20_FULLSCALE_500
	Acc_read(AccBuffer);  																//  Valeur initial(Zero) de accX, compensation avant le gyro important
		
	// Zone de calcul de accelerometre et gyroscope
	ZeroACCX = -37;
	Gyro_read(Buffer);
	Zerogyro = -Buffer[0];
	Anglegyro = 0;

	// Configuration de interruption de 94 Hz par accelerometre 
	Init_IT_EXT();																			//	Configuration d'interruption de EXTI pour gyro-acc à 94 Hz par default
	NVIC_SetPriority(EXTI1_IRQn, 2);										// 	Priorité 2  en IT du gyro

	// Initialisation et configuration de Bouton bleu et switche de presence user
	Init_Bouton();
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 
			
	while (1)										
	{
			/* Zone Attente IT */
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
