/**
  ******************************************************************************
  * @file    Capture
  * @author   
  * @version  
  * @date    juin 2013
  * @brief   Main program body
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_capture.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_lcd.h"
#include "stm32f0xx_delay.h"
 uint16_t mes_rpm (void);
uint32_t Frequence_CKINT;
uint32_t IC2Value;
uint8_t first_mesure = 1;
uint8_t etat_mesure =0 ;
/* initialisation timer 2en mode capture et "reset mode" du trigger

*/

void init_cap_tim2(void) 
{
	
TIM_ICInitTypeDef  TIM_ICInitStructure;
RCC_ClocksTypeDef RCC_Clocks;	
/* Valeur de la clock*/	

  RCC_GetClocksFreq(&RCC_Clocks);
	Frequence_CKINT = RCC_Clocks.HCLK_Frequency;
	
   /* _IRQ IT TIM2  Configuration_*/
  TIM_Config();
	
	/* Configuration capture canal 2*/
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; // canal 2
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;// détection front montant
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;// connecté sur IC2
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;// precaler diviseur = 1
  TIM_ICInitStructure.TIM_ICFilter = 0x0;// pas de filtre numérique
  TIM_ICInit(TIM2, &TIM_ICInitStructure);


  /* Select the TIM2 source trigger  Trigger: TI2FP2 */
  TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2  );

  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);
/*  UEV positionne le flag de débordement seulement sur un counter overflow sans action UG et raz trigger */
/*  UDIS =0 et URSS =1 */
  TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Regular);

	
  /* Activation du compteur */
  TIM_Cmd(TIM2, ENABLE);

  /* Enable masque source IT canal 2 capture CC2IE */
  TIM_ITConfig(TIM2, TIM_IT_CC2  , ENABLE);

    
}



/**
  * @brief  Configure  TIM2 IRQ Handler et compteur
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
		
  /* PRELIMINAIRE _ alimentation clock periph  TIM2 enable et a faire avant d'accéder au periphérique */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	
	/* confuguration compteur*/
	
	/* Inutile car c'est la valeur par défaut après activation de l'alimentation d'horloge du périphérique*/
	TIM_SetAutoreload(TIM2, 0xFFFFFFFF);//compteur principal max de la plage du 32 bits ARR
	
	/*Forçage de l'initialisation avec ug  */
  TIM_PrescalerConfig(TIM2 , 0,TIM_PSCReloadMode_Immediate); 
	
	TIM_SetClockDivision(TIM2, TIM_CKD_DIV1);// horloge échantillonnage filtrage  = CK_INT
	// compteur  inutile par défaut
  TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);	
 /* Lecture du registre de capture*/ 
}


/*Acquisitionn du registre de capture CC2*/
void itcapt(void)
{


	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	IC2Value = TIM_GetCapture2(TIM2);
	 etat_mesure = 1 ;
   
	if (first_mesure == 1) {
	  etat_mesure =0 ;
		first_mesure =0;
		IC2Value =0xFFFFFFFF;
	}
}



/* Conversion en tour/minute */ 

 uint16_t mes_rpm(void)
{
	
 uint32_t val_rpm;
	
/* un cycle compteur dure plusieurs dizaines de secondes
 * On démarre dans le main un cycle par effacement du drapeau de débordement
 * 
 * Le rafraichissement de l'affichage LCD dépend du nombre d'appel de cette fonction
 * La première mesure  ou celle qui suit  un débordement est considérée nulle 
*/
if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update)) {

	 TIM_ClearFlag(TIM2, TIM_FLAG_Update);// raz du flag de débordement
   first_mesure = 1;// première mesure s'annonce
	 etat_mesure =0 ;
	 return 0;
}
	if (first_mesure == 1) return 0;
  if (IC2Value == 0) return 0;

/* calcul RPM */
	val_rpm = (Frequence_CKINT*60) /IC2Value;
 
 

  if (val_rpm > 0xFFFF) return 0xFFFF;// max echelle > ou =
return  val_rpm;
 
}
	


	
//TIM_GetFlagStatus(TIM2, TIM_FLAG_Update);// retourne set ou reset  flag de débordement
//TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);// raz du flag de débordement




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

