/**
  ******************************************************************************
  * @file    Fonctions.c  
  * @author  INSA Toulouse
  * @version V1.0 
  * @date    13-Juin-2017 
  * @brief   Fonctions utilisé pour interruptions
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "Fonctions.h"


//Variable de gestion de ADC
// variables lectures ADC moyennes de Acq_ADC1[2]
__IO uint32_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;
// variables lectures ADC moyennees de Acq_ADC1[2]
__IO uint16_t MOY_ADC12_IN8 = 0, MOY_ADC12_IN9 =0;


//Variable de gestion de capteur gyro-acc
float ZeroACCX = 0.0f;														// variable de zero accelero X
float Zerogyro = 0.0f;														// variable de zero du gyro
float Angleacc = 0.0f;														// variable de l'angle a partir de tangente inverse
float Anglegyro = 0.0f;														// variable de l'angle a partir de d °/seconde
float Anglemesure = 0.0f;													// variable de l'angle fusionné
float VitesseAngle = 0.0f;												// variable de la vitesse angulaire
float AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};	// tableau pour lecture de valeurs par gyroscope et accelerometre

//Variable pour fonction bintoascii
unsigned char T[6]="";
unsigned char *chain_ascii = T;

//Variable pour fonction delay
__IO uint32_t TimingDelay = 0 ;										// variable pour fonction delay

//	Variable for gestion de consigne
float Consigne=0.0;																//	Consigne de courant de Raspberry Pi

//Variable pour fonction de arret trigger par Raspberry Pi
int cmpt_arr = 0 ;																//	variable de compteur pour reinitialiser état de arret de système 
int arret = 0;																		//	variable de l'état arrêt urgence declecnhé par Raspberry Pi 

// Variable pour mode entree angle beta 
int usage_acc= 1;

// Variable pour gestion de presence user 
int no_user = 0;
int presence_snd = 1;

// Variable pour matrice de fonction transfer
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
		presence_snd = 0;
		STM_EVAL_LEDOff(LED6);}
	else{
		presence_snd = 1;
		STM_EVAL_LEDOn(LED6);
	}
	
	/* 
		* Remis de l'état arret urgence 
		*	La detection de arret urgence est fait dans interruption de USART reception	
		* Si STM ne reçoit pas de trame arrêt pendant 3 secondes (300 itération car frenquence de envoie de trame par RPI = 100Hz)
		* Reinitialise the etat de arret 
	*/
	if(cmpt_arr>300){
		arret=0;
		cmpt_arr=0;
	}
	
	/* 
		* Choix de mode entree du systeme 
		*	Si 1 => utilisation de accelerometre pour entree de Beta avec LED 4 On
		* Si 0 => utilisation de potentiometre pour entree de Beta avec LED 4 Off
	*/
	if(usage_acc==1){
		angle_user=Anglemesure*PI/180.0f;
		STM_EVAL_LEDOn(LED4);
	}
	else{		
		angle_user=((float)(MOY_ADC12_IN8 *ScaleADC)/(0xFFF*75.0f)-20.0f)*PI/180.0f;
		STM_EVAL_LEDOff(LED4);
	}
	
	// Indication de variable arret urgence en LED 8  (Allumer si arret urgence = 1)
	if(arret >0){
		STM_EVAL_LEDOn(LED8);
	}
	else{
		STM_EVAL_LEDOff(LED8);
 	}
	
	// Calcul de systeme 
	angle_user_relatif=angle_user;
	xk0 = x_k[0]*A[0][0] + x_k[1]*A[0][1] + Consigne*B[0][0] + angle_user_relatif*B[0][1];
	xk1 = x_k[0]*A[1][0] + x_k[1]*A[1][1] + Consigne*B[1][0] + angle_user_relatif*B[1][1];
	x_k[0] = xk0;
	x_k[1] = xk1;
	
	/* Envoie de donnée par USART dans le forme suivante*/

	tampon_float = x_k[0]; 								//en radians 
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
	
	tampon_float = x_k[1]; 								//en radians 
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label s, vitesse angulaire
	TX_USART [8] = '<';
	TX_USART [9] = 's';
	TX_USART [10] =  tampon_char[0];		
	TX_USART [11] = tampon_char[1];		
	TX_USART [12] = tampon_char[2];		
	TX_USART [13] = tampon_char[3];		
	TX_USART [14] = '\n';
	
	tampon_float=(float)(MOY_ADC12_IN9 *ScaleADC)/(0xFFF*30.0f);	//en pourcentage 
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label b, niveau de batterie
	TX_USART [15] = '<';
	TX_USART [16] = 'b';
	TX_USART [17] = tampon_char[0];		
	TX_USART [18] = tampon_char[1];		
	TX_USART [19] = tampon_char[2];		
	TX_USART [20] = tampon_char[3];		
	TX_USART [21] = '\n';
	
	tampon_char=(unsigned char*)&angle_user; 								//en radians 
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
	
	DMA_Cmd(DMA1_Channel7, DISABLE);													// 	Arret
	DMA_SetCurrDataCounter(DMA1_Channel7, sizeof(TX_USART));	//	Mise au depart pointeur sur Buffer en arret DMA
	DMA_Cmd(DMA1_Channel7, ENABLE);

	//	Limitation de 180.0 deg à -180.0 deg
	if(x_k[0] > 1.57f || x_k[0] < -1.57f) {
		Consigne=0;
    x_k[0] = 0;
    x_k[1] = 0;
    u_k[0] = 0;
    u_k[1] = 0;;
	}
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

/**
  * @brief  Fonction de convetir en ASCII
  * @param  None
  * @retval None
  */
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

/**
  * @brief  Fonction de convetir en bytes à float (pour reception de donnée)
  * @param  None
  * @retval None
  */
float bytes_to_float(unsigned char * bytes)
{
    unsigned long value;
    value = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0]);
    return *(float *)&value;
}
/************************ (C) COPYRIGHT INSA TOULOUSE *****END OF FILE****/
