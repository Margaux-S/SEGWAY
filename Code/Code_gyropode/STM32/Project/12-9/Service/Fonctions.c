/**
  ******************************************************************************
  * @file    Fonctions.c  
  * @author  INSA Toulouse
  * @version V1.0 
  * @date    12 Septembre 2017
  * @brief   Fonctions utilisé pour interruptions
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "Fonctions.h"

/*Correcteur PI du courant */
/*
*=============================================================================================
* 					MODELE AUTOMATIQUE de boucle de courant 
*=============================================================================================
*
*
*=====Fonction de transfert du correcteur PI analogique
* C(p) = (Kp*(1+Ti.p))/Ti.p
*
*
*Consigne de courant signé I , est exprimée en I!
*u est le rapport cyclique [0 1 ] 
*
*
*
*	  	(en A)				err(enA)	  ________	  v (en V) 	_________________  u 		  	 ______   Commande (en V)   ______
* Consigne_I-->+( )----------->| Cor_PI |--->--------| v/(2*Ubatt)+0.5 |---->-----| PWM  |-------->----------|Moteur|--->------- I
*	 						 -^     			 	 |________|            |_________________|          |______|                   |______|				|
*							  |																																																					  |
*							  |     ________                                       																											  |
*							  |-<- |RetourI |--<------------------------------------------------------------------------------------------|
*								 		 |________|
*
*Dans notre cas, on a déjà prendre en compte des gains de convertisseur de capteur de courant avec calcul suivant: 
*
*==============ZONE DE CAL LT6  ===================   
*
*V(in ADC) =(1/COEF)  X { V0 +(.625/6) X IM}  formule brute
*V0 sortie capteur à vide , voisine de 2.5 V soit V01 ou V02 pour moteur 1 & 2
*COEF : attenuation du pont diviseur de resitances ( en sortie LTS6 et entree ADC) soit COEF1&2 por moteur 1 & 2
*	 
*	 on pose VIM10 ou VIM20 = V01(2) * 1/COEF1(2) tension mesurée in ADC, à vide
*  V = VIM10(20)+ 1/COEF1(2) X 0.625/6 X IM1(2)) en sortie pont diviseur et entrée ADC
*	 Moteur 1 ou 2 :	
*	 Les tensions mesurées sont en mV de 0 à 3000 mv, dynamique ADC choisi
*	 IM1&2 =(V-VIM10)10^-3 X 9.6 X 100/ COEF1(2) donne format XX.XX  virgule ( X 1/100)
*			=0.96 (V-V1M10)X COEF1(2)
*
*===================================================
*
* Dimensionment PI analog
* Ti = L/R
* Kp = 2*R
*/

//Variable de gestion de ADC
__IO uint32_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;	//variable temporaire pour lecture valeurs de ADC
// variables lectures ADC moyennees de Acq_ADC1[6]
__IO uint16_t MOY_ADC12_IN6 = 0, MOY_ADC12_IN7 = 0, MOY_ADC12_IN8 = 0, MOY_ADC12_IN9 =0,MOY_ADC1_IN2 =0,MOY_ADC1_IN5 =0;
__IO int32_t  IM1,IM2,VIM10,VIM20;																//valeur intern pour capteur de courant de moteur

//Variable de gestion de capteur gyro-acc
float ZeroACCX = 0.0f;														// variable de zero accelero X
float Zerogyro = 0.0f;														// variable de zero du gyro
float Angleacc = 0.0f;														// variable de l'angle a partir de tangente inverse
float Anglegyro = 0.0f;														// variable de l'angle a partir de d °/seconde
float Anglemesure = 0.0f;													// variable de l'angle fusionné
float VitesseAngle = 0.0f;												// variable de la vitesse angulaire
float AccBuffer[3] = {0.0f}, Buffer[3] = {0.0f};	// tableau pour lecture de valeurs par gyroscope et accelerometre

//Variable de vitesse lineaire du système
float VitesseLineaire=0;
float tampon_vit=0;																	// variable  pour moyenner la vitesse lineaire  
int compt_vit=0; 																	// variable de compteur pour moyenner la vitesse lineaire  

//	Variable for gestion de consigne
float Consigne1=0.0,Consigne2=0.0;								// variable de consigne de courant pour boucle de courant 
float Consigne_new=0.0;														// variable de consigne reçu de Raspberry Pi 
int Consigne_OK=0; 																// etat de reception de consigne de Raspberry Pi (1 si reçu, 0 sinon)

// variable de l'état de la communication android
int android = 0;
// variable de la consigne d'angle d'android
float angleAndroid = 90.0;

//	Variable interne pour boucle de courant
float v1_k=0.0,v2_k=0.0,v1_k_a=0.0,v2_k_a=0.0;
float u1_k=0.0,u2_k=0.0;
float err1=0.0, err2=0.0, err1_a=0.0, err2_a=0.0, integral1 = 0.0, integral2 = 0.0;
float a1;
float a0;

// Variable pour gestion de presence user 
int no_user = 0;																	//	variable de compteur pour delay la reponse de bouton presence user 
int presence_snd = 1;															//	variable de etat de prensence user 

//Variable pour fonction de arret trigger par Raspberry Pi
int cmpt_arr = 0 ;																//	variable de compteur pour reinitialiser état de arret de système 
		
//Variable pour fonction bintoascii
unsigned char T[6]="";
unsigned char *chain_ascii = T;

//Variable pour fonction delay
__IO uint32_t TimingDelay = 0 ;		// variable pour fonction delay


/**
  * @brief  Fonction principal pour traitement de donnée Gyro-Acc, et Envoie de donnée à Raspberry Pi, des gestions de precaution est fait
  * @param   
  * @retval  
  */
void Trait_Gyro_Acc (void) {
	
	unsigned char *tampon_char;
	float tampon_float=0.0f;

	Gyro_read(Buffer);
	Acc_read(AccBuffer);
	AccBuffer[0] = AccBuffer[0] + ZeroACCX;	
	AccBuffer[2] = ABS (AccBuffer[2]);											// 	Acceleration z ne peut etre négative sur gyropode phase change de signe 
 	Angleacc  =-atan (AccBuffer[0]/AccBuffer[2])*180/PI ;		//	Valeur en degree
	Anglemesure	= 0.98f *(Anglemesure	+	(Deltatps * (Zerogyro+Buffer[0])))	+0.02f*Angleacc;	//	Calcul de fusion d'angle
	VitesseAngle = (Zerogyro+Buffer[0]);																											
	
	/* 
		* Precaution de presence user 
		* Zone de tolerance pour la non présence de user = 0.5s = 500ms
		* Ce boucle est de frequence de 94 Hz => il faut itération de 47 fois pour atteindre 500ms
		* La detection est fait comme suivant:
		*			Si le bouton est appuyé, la valeur lu est 0, l'état de presnece user est 1
		*			Si le bouton est relacher, la valeur lu est 1, le compteur va augmenter
		*				Si le compteur est à 47, il indique l'absence de appuie de bouton pour 0.5s, donc état de presnece =0 
	*/
	
	// Lecture de GPIO 4 pour bouton presnece user 
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
	
	/* 
		* Remis de l'état arret urgence 
		*	La detection de arret urgence est fait dans interruption de USART reception	
		* Si STM ne reçoit pas de trame arrêt pendant 3 secondes (300 itération car frenquence de envoie de trame par RPI = 100Hz)
		* Reinitialise the etat de arret 
	*/
	if(cmpt_arr>100){
		arret=0;
		cmpt_arr=0;
	}
	
	/* 
		* Gestion de même reception de consigne 
		* Precaution pour eviter on recoit plus de 10 fois de même consigne qui peut être convoqué par probleme de communication 
		*	Si c'est le cas, STm 32 prend le relai asservissement lui même 
	*/
	
	if (cmpt_egal >= 10){ 
		Trait_consigne();
		STM_EVAL_LEDOn(LED5);
	}
		
	/* Envoie de donnée par USART dans le forme suivante*/
	
	tampon_float = Anglemesure*(PI/180.0f); 			// Conversion en radians de angle
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
	
	tampon_float = VitesseAngle*(PI/180.0f);			// Conversion en radians par seconde de vitesse angulaire
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label s, vitesse angulaire
	TX_USART [8] = '<';
	TX_USART [9] = 's';
	TX_USART [10] = tampon_char[0];		
	TX_USART [11] = tampon_char[1];		
	TX_USART [12] = tampon_char[2];		
	TX_USART [13] = tampon_char[3];		
	TX_USART [14] = '\n';
	
	 
	// Mesure tension batterie PC3 MOY_ADC12_IN9 
	tampon_float=(MOY_ADC12_IN9 *ScaleADC)/(0xFFF*30); // Division par echelle de ADC et rapport de 30 pour convertir en pourcentage 
	tampon_char=(unsigned char*)&tampon_float;
	//Envoie de message de label b, niveau de batterie
	TX_USART [15] = '<';
	TX_USART [16] = 'b';
	TX_USART [17] = tampon_char[0];		
	TX_USART [18] = tampon_char[1];		
	TX_USART [19] = tampon_char[2];		
	TX_USART [20] = tampon_char[3];		
	TX_USART [21] = '\n';
	
	
	tampon_char=(unsigned char*)&VitesseLineaire;
	//Envoie de message de label v, vitesse 
	TX_USART [22] = '<';
	TX_USART [23] = 'v';
	TX_USART [24] = tampon_char[0];		
	TX_USART [25] = tampon_char[1];		
	TX_USART [26] = tampon_char[2];		
	TX_USART [27] = tampon_char[3];		
	TX_USART [28] = '\n';	
	

	tampon_float=(float)presence_snd;
	tampon_char = (unsigned char*)&tampon_float;
	//Envoie de message de label t, Status
	TX_USART [29] = '<';
	TX_USART [30] = 'u';
	TX_USART [31] = tampon_char[0];		
	TX_USART [32] = tampon_char[1];		
	TX_USART [33] = tampon_char[2];		
	TX_USART [34] = tampon_char[3];		
	TX_USART [35] = '\n';		
	TX_USART [36] = 'X';
	

	DMA_Cmd(DMA1_Channel7, DISABLE);													// 	DISABLE de USART 
	DMA_SetCurrDataCounter(DMA1_Channel7, sizeof(TX_USART));	//	Mise au depart pointeur sur Buffer en arret DMA
	DMA_Cmd(DMA1_Channel7, ENABLE);														// 	Enable de USART 
	
}

/**
  * @brief  Traitement de boucle de courant de type correcteur PI
  * @param   
  * @retval  
  */
void Trait_b_courant (void) {


	GPIO_SetBits(GPIOC,SHDNM1 | SHDNM2);										// 	moteurs 1&2 ON

	ADC1ConvertedValue =  MOY_ADC12_IN6;										// 	Courant de moteur 1 resultat en mv 3000 max
	IM1 = (int32_t)(0.96f * COEFI1 * ((float) (ADC1ConvertedValue *ScaleADC/0xFFF) - VIM10));
					
	ADC1ConvertedValue =  MOY_ADC12_IN7;										// 	Courant de moteur 2 resultat en mv 3000 max
	IM2 = (int32_t)(0.96f * COEFI2 * ((float) (ADC1ConvertedValue *ScaleADC/0xFFF)- VIM20));
	
	
	/* 
		* Gestion de reception de trame arret urgence de Raspberry Pi
		*	La detection de arret urgence est fait dans interruption de USART reception	
		* STM 32 va interprete le signe consinge de courant et le diminuer jusau'au zéro
		* En realité, quand STM 32 recoit le trame de arrêt, il decelerate jusqu'au l'arrêt de moteur 
		*	Si on ne reçoit pas de trame de arret, STM va utiliser le consigne de Raspberry Pi 
	*/
	if (arret == 1){
		STM_EVAL_LEDOn(LED10);
		if(Consigne1 >= -0.02f && Consigne1 <= 0.02f){
			Consigne1 = 0.0f;
			Consigne2 = 0.0f;
		}
		else if(Consigne1 > 0.0f){
			Consigne1 -= 0.001f;
			Consigne2 -= 0.001f;
		}
		else if(Consigne1 < 0.0f){
			Consigne1 += 0.001f;
			Consigne2 += 0.001f;
		}
	}else{
		STM_EVAL_LEDOff(LED10);
		if (Consigne_OK==1){			// Si c'est bien une nouvelle trame reçu
			if (android){						// Si la communication android est établie
				if (angleAndroid < 90){	//	On a donc une valeur d'angle			 
					Consigne2 = Consigne_new;
					Consigne1 = (angleAndroid/90)*Consigne_new;
				} else if (angleAndroid >= 90){
					Consigne2 = ((180.0-angleAndroid)/90)*Consigne_new*0.5;
					Consigne1 = Consigne_new;
				}			
				Consigne_OK = 0;
			} else {
				Consigne1=Consigne_new;
				Consigne2=Consigne_new; 
				Consigne_OK = 0;
			} 
		}
	}

	
	
	//partie de traitement de direction à refaire
	
	/*

 		// Prendre la valeur de tension dans guidon [1950 ; 2560 ; 2910]
  		ADC1ConvertedValue =  MOY_ADC12_IN8;//IN8
 		ADC1ConvertedVoltage = (ADC1ConvertedValue *ScaleADC)/0xFFF; // Conversion en tension (1950-2910)
 		
 		//cas 1 : La valeur lu est dehors des valeurs limitées
 		if ((ADC1ConvertedVoltage < 1930) &&(ADC1ConvertedVoltage > 2920)){
 				Consigne1=0.0f;
 				Consigne2=0.0f;
 		}
 			
 		if(Anglemesure < 4.0f){
 			//cas 3 : La valeur lu est dans la zone de droite (2570-2910) [0°; 10°]
 			if ((ADC1ConvertedVoltage <= 2910) &&(ADC1ConvertedVoltage >= (GUIDON50))){
 					Tampon = (float)(ADC1ConvertedVoltage-(GUIDON50))/34.0f; // Conversion en degrés
 					Consigne2 = Consigne + Tampon * 0.25f;
 					Consigne1 = Consigne - Tampon * 0.25f;
 			}
 				
 			//cas 4 : La valeur lu est dans la zone de gauche (1950-2550) [-10°; 0°]
 			if ((ADC1ConvertedVoltage >= 1700) &&(ADC1ConvertedVoltage <= (GUIDON50))){
 					Tampon = (float)((GUIDON50)-ADC1ConvertedVoltage)/60.0f; // Conversion en degrés, jauge non symétrique
 					Consigne1 = Consigne + Tampon * 0.25f;
 					Consigne2 = Consigne - Tampon * 0.25f;
 			}
 		}
 		
 		else{
 			//cas 3 : La valeur lu est dans la zone de droite (2570-2910) [0°; 10°]
 			if ((ADC1ConvertedVoltage <= 2910) &&(ADC1ConvertedVoltage >= (GUIDON50))){
 					Tampon = (float)(ADC1ConvertedVoltage-(GUIDON50))/34.0f; // Conversion en degrés
 					Consigne2 = Consigne - Tampon * 0.25f;
 					Consigne1 = Consigne + Tampon * 0.25f;
 			}
 				
 			//cas 4 : La valeur lu est dans la zone de gauche (1950-2550) [-10°; 0°]
 			if ((ADC1ConvertedVoltage >= 1700) &&(ADC1ConvertedVoltage <= (GUIDON50))){
 					Tampon = (float)((GUIDON50)-ADC1ConvertedVoltage)/60.0f; // Conversion en degrés, jauge non symétrique
 					Consigne1 = Consigne - Tampon * 0.25f;
 					Consigne2 = Consigne + Tampon * 0.25f;
 			}
 		}
		*/
		
		
	// En realité est autour de 14A , a reverifier 
	//precaution, limitation de consigne de courant, precaution de protection de hacheur 
	if (Consigne1 >10.0f)
		Consigne1=10.0f;
	else if (Consigne1<-10.0f)
		Consigne1=-10.0f;

	if (Consigne2 >10.0f)
		Consigne2=10.0f;
	else if (Consigne2<-10.0f)
		Consigne2=-10.0f;
	

	/*	Correcteur Gain Proportionnel Integral 	*/
		err1 = (Consigne1 - (float)IM1/100.0f); // err dans unité de courant 
		err2 = (Consigne2 - (float)IM2/100.0f);

		a1=Kp*(Te/(2.0f*Ti)+1.0f);
		a0=Kp*(Te/(2.0f*Ti)-1.0f);
		
		v1_k=(v1_k_a+a1*err1+a0*err1_a) ;
		v2_k=(v2_k_a+a1*err2+a0*err2_a) ;
	
	//precaution, limitation de tension de courant, precaution de protection de hacheur 
	if (v1_k >36.0f)
		v1_k=36.0f;
	else if (v1_k<-36.0f)
		v1_k=-36.0f;

	if (v2_k >36.0f)
		v2_k=36.0f;
	else if (v2_k<-36.0f)
		v2_k=-36.0f;
	
	u1_k= v1_k/72.0f+0.5f;
	u2_k= v2_k/72.0f+0.5f;
	
	//	Décalage entrée sortie
	err1_a=err1;
	err2_a=err2;
	v1_k_a=v1_k;
	v2_k_a=v2_k;

	//precaution, limitation de rapport cyclique de PWM, precaution de protection de hacheur 
	if (u1_k >0.9f)
		u1_k=0.9f;
	else if (u1_k<0.1f)
		u1_k=0.1f;

	if (u2_k >0.9f)
		u2_k=0.9f;
	else if (u2_k<0.1f)
		u2_k=0.1f;
	PWM_MOTEUR1 = (2.0f* PWM50*(u1_k));			// conversion en %
	PWM_MOTEUR2 = (2.0f* PWM50*(u2_k));			// conversion en %

	TIM_SetCompare1( TIM1,  PWM_MOTEUR1);		// ecriture de CCR1 de 16 bits
	TIM_SetCompare2( TIM1,  PWM_MOTEUR2);		// ecriture de CCR2 de 16 bits		
	
	if (compt_vit> 20){
		VitesseLineaire= tampon_vit/20.0f;
		compt_vit=0;
	}else{
		tampon_vit = tampon_vit + (((2.0f * u1_k - 1.0f) - 0.47f * IM1) * Rw) / (Km * Kg);
		compt_vit++;
	}

}

/**
  * @brief  Assservisement de système par STM 32 lui-même 
  * @param   
  * @retval  
  */
void Trait_consigne(void){
	
	float Consigne;
		Consigne =(k1*Anglemesure+k2*VitesseAngle)*PI/(180.0f*0.874995f); //PD
		Consigne2 = Consigne ;
		Consigne1 = Consigne ;
	
		//Saturateur
		if (Consigne1 >10.0f)
			Consigne1=10.0f;
		else if (Consigne1<-10.0f)
			Consigne1=-10.0f;

		if (Consigne2 >10.0f)
			Consigne2=10.0f;
		else if (Consigne2<-10.0f)
			Consigne2=-10.0f;
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
