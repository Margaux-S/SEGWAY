/**
  ******************************************************************************
  * @file    Fonctions.c  
  * @author  INSA Toulouse
  * @version V1.0 
  * @date    13-Juin-2017 
  * @brief    
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "Fonctions.h"

// variables lectures ADC moyennes de Acq_ADC1[4]
__IO uint32_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
__IO uint16_t MOY_ADC12_IN8 = 0, MOY_ADC12_IN9 =0;// variables lectures ADC moyennees de Acq_ADC1[4]


float ZeroACCX = 0.0f;			// zero accelero X
float Zerogyro = 0.0f;			// zero du gyro
float Angleacc = 0.0f;			// calcule de l'angle a partir de tangente inverse
float Anglegyro = 0.0f;			// calcule de l'angle a partir de d °/seconde
float Anglemesure = 0.0f;		// calcule de l'angle fusionné
float VitesseAngle = 0.0f;	// vitesse angulaire
float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};

int secu = 0;
	
unsigned char T[6]="";
unsigned char *chain_ascii = T;
__IO uint32_t TimingDelay = 0;

float Consigne=0.0;				//	Consigne de courant de Raspberry Pi

int arret = 0;
int usage_acc= 1;
// Status envoyé à RPI 
int status_snd = 0;
int status_rcv = 0;
int no_user = 0;
int presence_snd = 1;

bool Consigne_OK=false; 

float A[2][2] = {1.003f, 0.02002f,0.3166f, 1.003f};
float B[2][2] = {-0.0002483f, 0.00312f, -0.02485f, 0.3122f};
float x_k[2] = {0,0};
float u_k[2] = {0,0};


/**
  * @brief  Traitement données de Gyro, Accelero et ADC
  * @param   
  * @retval  
  */
void Trait_Gyro_Acc (void) {
	
	unsigned char *tampon_char;
	float tampon_float=0.0f;
	float angle_user=0.0f;
	float angle_user_relatif=0.0f;
	float xk0=0.0f;
	float xk1=0.0f;

	Gyro_read(Buffer);
	
	Acc_read(AccBuffer);
	AccBuffer[0] = AccBuffer[0] + ZeroACCX;	
	AccBuffer[2] = ABS (AccBuffer[2]);// Acceleration z ne peut etre négative sur gyropode phase change de signe 
 	Angleacc  =-atan (AccBuffer[0]/AccBuffer[2])*180/PI ;// - atan( AccX/AccZ)X 180/PI
	Anglemesure	= 0.98f *(Anglemesure	+	(Deltatps * (Zerogyro+Buffer[0])))	+0.02f*Angleacc;//0.98 + 0.02 = 1
	VitesseAngle = (Zerogyro+Buffer[0]);// vitesse angulaire


	
	// Lecture du PA4 : présence user
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)){
		no_user++;
		STM_EVAL_LEDOff(LED6);}
	else{
		no_user = 0;
		presence_snd = 1;
		STM_EVAL_LEDOn(LED6);
	}
	
	if(no_user >= 47){
		presence_snd = 0;
	}
	
	if(usage_acc==1){
		angle_user=Anglemesure*PI/180.0f;
	}
	else{		
		angle_user=((float)(MOY_ADC12_IN8 *ScaleADC)/(0xFFF*75.0f)-20.0f)*PI/180.0f;
	}
	
	angle_user_relatif=angle_user- x_k[0];

		xk0 = x_k[0]*A[0][0] + x_k[1]*A[0][1] + Consigne*B[0][0] + angle_user_relatif*B[0][1];
		xk1 = x_k[0]*A[1][0] + x_k[1]*A[1][1] + Consigne*B[1][0] + angle_user_relatif*B[1][1];
		x_k[0] = xk0;
		x_k[1] = xk1;

	tampon_float = x_k[0]; /* Conversion en radians */
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label p, position angulaire
	TX_USART [0] = 'R';
	TX_USART [1] = '<';
	TX_USART [2] = 'p';
	TX_USART [3] = tampon_char[0];		
	TX_USART [4] = tampon_char[1];		
	TX_USART [5] = tampon_char[2];		
	TX_USART [6] = tampon_char[3];		
	TX_USART [7] = '\n';	
	
	tampon_float = x_k[1]; /* Conversion en radians */
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label s, vitesse angulaire
	TX_USART [8] = '<';
	TX_USART [9] = 's';
	TX_USART [10] =  tampon_char[0];		
	TX_USART [11] = tampon_char[1];		
	TX_USART [12] = tampon_char[2];		
	TX_USART [13] = tampon_char[3];		
	TX_USART [14] = '\n';
	
	tampon_float=(float)(MOY_ADC12_IN9 *ScaleADC)/(0xFFF*30.0f);
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label b, niveau de batterie
	TX_USART [15] = '<';
	TX_USART [16] = 'b';
	TX_USART [17] = tampon_char[0];		
	TX_USART [18] = tampon_char[1];		
	TX_USART [19] = tampon_char[2];		
	TX_USART [20] = tampon_char[3];		
	TX_USART [21] = '\n';
	
	tampon_float=	((float)(MOY_ADC12_IN8 *ScaleADC)/(0xFFF*75.0f)-20.0f)*PI/180.0f;
	tampon_char=(unsigned char*)&tampon_float;
	TX_USART [22] = '<';
	TX_USART [23] = 'v';
	TX_USART [24] = tampon_char[0];		
	TX_USART [25] = tampon_char[1];		
	TX_USART [26] = tampon_char[2];		
	TX_USART [27] = tampon_char[3];		
	TX_USART [28] = '\n';	
	
	tampon_float=(float)presence_snd;
	tampon_char = (unsigned char*)&tampon_float;
	TX_USART [29] = '<';
	TX_USART [30] = 'u';
	TX_USART [31] = tampon_char[0];		
	TX_USART [32] = tampon_char[1];		
	TX_USART [33] = tampon_char[2];		
	TX_USART [34] = tampon_char[3];		
	TX_USART [35] = '\n';		
	TX_USART [36] = 'X';
	

	
	if(arret){
		STM_EVAL_LEDOn(LED4);
	}
	else{
		STM_EVAL_LEDOff(LED4);
	}
	
	DMA_Cmd(DMA1_Channel7, DISABLE);													// 	Arret
	DMA_SetCurrDataCounter(DMA1_Channel7, sizeof(TX_USART));	//	Mise au depart pointeur sur Buffer en arret DMA
	DMA_Cmd(DMA1_Channel7, ENABLE);

	 
	/*************************************************************************************************************************************/

}



/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

unsigned char * bintoascii(uint16_t data_bin)
{
	*(chain_ascii + 5)     = 0;
	*(chain_ascii + 4) = 0x30 + data_bin %10 ;// unité
	data_bin /= 10;
  *(chain_ascii + 3) = 0x30 + data_bin %10 ;// dizaine
	data_bin /= 10;
  *(chain_ascii + 2) = 0x30 + data_bin %10 ;// centaine
	data_bin /= 10;
  *(chain_ascii + 1) = 0x30 + data_bin %10 ;// unité de 1000
	data_bin /= 10;
  *(chain_ascii)     = 0x30 + data_bin %10 ;// 10 aine de 1000

	return chain_ascii;

}




float bytes_to_float(unsigned char * bytes)
{
    unsigned long value;
    value = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0]);
    return *(float *)&value;
}
/************************ (C) COPYRIGHT INSA TOULOUSE *****END OF FILE****/
