/**
  ******************************************************************************
  * @file    stm32f0xx_delay.c
  * @author  Perso
  * @version V1.0.0
  * @date    mai-2013
  * @brief   Pilotage d'un afficheur LCD 1 ou 2 lignes  
  *          
  *            
  *            
  *            
  *           
  *
  *******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_delay.h"
  
  
  /*__variable globale au fichier__*/
static __IO uint32_t TimingDelay;// pour ne pas être modifier en drhors du fichier
//__IO uint32_t TimingDelay;
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	 
		if (TimingDelay != 0x00)
			{ 
				TimingDelay--;
			}
}
