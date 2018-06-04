/**
  ******************************************************************************
  * @file    Init.c
  * @author  INSA Toulouse
  * @version V1.0
  * @date    13-Juin-2017
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Init.h"

/* Variable type structure ---------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
GPIO_InitTypeDef  GPIO_InitStructure;	
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef       ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
DMA_InitTypeDef DMA_InitStructure;

/* Variable ------------------------------------------------------------------*/
__IO uint16_t Acq_ADC1[2];
__IO uint16_t calibration_value=0;
uint8_t TX_USART [37] = "<p....n<s....n<b....n<v....n<t....n"; //Buffer emission 		
uint8_t RX_USART [7]   = ""; //Buffer reception	



/**
* @brief Initialisations d'interruption RXTI pour gyroscope et accelerometre
* @param None
*/
void Init_IT_EXT(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE); 								// Enable GPIOE clock
  
  // Configure PE1 pin as input floating 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  // Enable SYSCFG clock 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  // Connect EXTI1 Line to PE1 pin 
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

  // Configure EXTI1 line
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;													//	Ligne 1 bit 1 PE1 INT2 accelero
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // Enable and set EXTI0 Interrupt to the lowest priority 
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;										//	EXTI0_IRQn
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
* @brief Initialisations d'interruption DMA pour ADC
* @param None
*/
void Init_IT_DMA1(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);//activation
}

//PortA PA4 Bouton présence user
void Init_Bouton(void)
{
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4; // GPIO_Pin_4 PortA
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/**
* @brief Initialisations ADC en DMA
* @param None
*/
void Init_ADC1_DMA1(void)
{	
  /* ADC Channel configuration */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);//clock port C

  /* Configure ADC1 : Channel 8,9 Port C */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;//GPIO_Pin PortC
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/*configuration DMA1 associé à ADC1 Cycle complet 5 mesures > 700Hz*/
	/* Enable DMA1clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);// adresse du registre data sortie ADC1
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Acq_ADC1;// adresse de destinacion dans la mémoire 
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;// le peripherique est la source vers la mémoire
  DMA_InitStructure.DMA_BufferSize = 2;// taille du buffer pour IN8 à IN9 2 mesures
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//incrementation registre peripherique non
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//incrementation adresse memoire oui
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16 bits
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;// 16 bits
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// circulaire
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;// memoire a memoire non

  DMA_Init(DMA1_Channel1, &DMA_InitStructure);// chanel1 DMA1 dispo pour ADC1  
	
  /* Calibration procedure */  
  ADC_StructInit(&ADC_InitStructure);//defaut

  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  

  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  calibration_value = ADC_GetCalibrationValue(ADC1);
	
  /* Configure ADC1 : Channel 8,9 Port C */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent  ;//  ADC_Mode_Independent                                                           
  ADC_CommonInitStructure.ADC_Clock =ADC_Clock_AsynClkMode   ;// ou ADC_Clock_SynClkModeDiv1,2...                    
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled  ;  //ADC1_CCR    MDMA[1:0] bit 14 & 15   for dual mode Em mode simpe -->  off   
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;// circulaire                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
  ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
  
  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;//
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;// 
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;//
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel = 2;// 2 voies
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC1 regular channel 8,9 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_19Cycles5);//PC2 ADC12_IN8 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_19Cycles5 );//PC3 ADC12_IN9

	/* Configures the ADC DMA */
  ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);
  /* Enable the ADC DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* wait for ADRDY */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
  	  
	/* Enable the DMA channel */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  /* Start ADC1 Software Conversion */ 
  ADC_StartConversion(ADC1);   
 	
}

/**
* @brief Initialisations USART en DMA
* @param None
 */
void Init_USART(void)
{
  NVIC_InitTypeDef      NVIC_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);				//	RX USART2 PA3
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);				//	TX USART2 PA2
	
  // Configure USART1 pins:  Rx and Tx
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  // Activation IT en reception- Enable USART1 IRQ 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  USART_InitStructure.USART_BaudRate =115200; 															//	Vitesse Transmission BaudRate=115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;								// 	8 bits ou 9 possible
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						// Activation des lignes

  USART_Init(USART2, &USART_InitStructure);
  
  USART_Cmd(USART2,ENABLE);// activation USART2
	
	//	 DMA configuration pour USART	
  DMA_DeInit(DMA1_Channel7);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST ; 											//	 Emission
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TX_USART;								//	TX_USART
  DMA_InitStructure.DMA_BufferSize = (uint16_t)sizeof(TX_USART);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->TDR);			// Emission 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal  ;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel7, &DMA_InitStructure);

  USART_DMACmd(USART2, USART_DMAReq_Tx , ENABLE);	 													//	Enable the USART Tx DMA request 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 														//	Activation reception ITM enum RX_USART
}
