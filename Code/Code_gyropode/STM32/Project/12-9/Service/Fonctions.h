/**
  ******************************************************************************
  * @file    Fonctions.h 
  * @author  INSA Toulouse
  * @version V1.0
  * @date    13-Juin-201
  * @brief   Header for Fonctions.c
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONCTIONS_H
#define __FONCTIONS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "Gyroscope.h"
#include "Accelerometre.h"
#include "Init.h"
#include "math.h"

/* Private define ------------------------------------------------------------*/
#define PI                         (float)     3.14159265f 
#define Deltatps                   (float)     0.010526f        // Delta t du Gyro  = 1/94 HZ 
#define ScaleADC                    3000                        //pleine echelle ADC 3V 
#define COEFI1  (float) 1.71283f 		// attenuateur capteur courant moteur 1  = 2.523/1.473 sortie capteur/ entrée mesure à vide in ADC
#define COEFI2  (float) 1.71584f 		// attenuateur capteur courant moteur 2  = 2.512/1.464 sortie capteur/ entrée mesure à vide in ADC

//	Define de coefficient de boucle de courant 
#define R 0.47f							// Resistance de moteur 
#define L 0.00047f					// Inductance de bobine de moteur 
#define Kp R								// Gain de correcteur de P
#define Ti L/R							// Constant de temps de correcteur de I
#define Ki 1.0f/Ti					// Gain de correcteur de I
#define Freq_e  5000				// Frequence de echantillonage pour boucle de courant qui correspondant à frequence de IT DMA ADC
#define Te 1.0f/Freq_e			

//	Define de coefficient de systeme 
#define Km 0.1375f	
// Constante de couple du moteur (C = Km * I) 
#define Kg 6.3636f
#define Rw 0.127f

//	Define de coefficient de boucle de consigne (gain de correcteurs)
#define k1 -25.8598f  //-12.9015f sans user //-25.8598f avec user 
#define k2 -6.5593f //-4.0952f sans user //-6.5593f avec user 
    	
//	Define de valeur milieu de tension pour le guidon  
#define GUIDON50 2560

/* Global variables ---------------------------------------------------------*/

//Variable de gestion de ADC
extern __IO uint32_t  ADC1ConvertedValue , ADC1ConvertedVoltage;  		//variable temporaire pour lecture valeurs de ADC
// variables lectures ADC moyennees de Acq_ADC1[6]
extern __IO uint16_t MOY_ADC12_IN6 , MOY_ADC12_IN7 , MOY_ADC12_IN8 , MOY_ADC12_IN9 ,MOY_ADC1_IN2 ,MOY_ADC1_IN5 ;
//valeur intern pour capteur de courant de moteur
extern __IO int32_t  IM1,IM2,VIM10,VIM20;

//Variable de gestion de capteur gyro-acc
extern float ZeroACCX ;								// variable de zero accelero X
extern float Zerogyro ;								// variable de zero du gyro
extern float Angleacc ;								// variable de l'angle a partir de tangente inverse
extern float AccBuffer[3], Buffer[3];	// tableau pour lecture de valeurs par gyroscope et accelerometre
extern float Anglegyro;								// variable de l'angle a partir de d °/seconde
extern float Anglemesure;							// variable de l'angle fusionné
extern float VitesseAngle;						// variable de la vitesse angulaire

/*Variable for gestion de consigne */
extern float Consigne1;								// variable de consigne de courant pour boucle de courant de moteur 1
extern float Consigne2;								// variable de consigne de courant pour boucle de courant de moteur 2
extern float Consigne_new;						// variable de consigne reçu de Raspberry Pi 
extern int cmpt_egal;									// variable de compteur pour la meme consigne detecte ( raison securité) 
extern int Consigne_OK; 							// variable de etat de reception de consigne de Raspberry Pi (1 si reçu, 0 sinon)
extern int android;
extern float angleAndroid;

//Variable pour fonction delay
extern __IO uint32_t TimingDelay ;		// variable pour fonction delay

//Variable pour fonction de arret trigger par Raspberry Pi
extern int arret;											//	variable de l'état arrêt urgence declecnhé par Raspberry Pi 
extern int cmpt_arr;									//	variable de compteur pour reinitialiser état de arret de système 

/* Private macro -------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x

/* Exported functions ------------------------------------------------------- */
void Trait_Gyro_Acc (void);
void Trait_b_courant (void) ;
void Trait_consigne(void);
unsigned char * bintoascii(uint16_t data_bin);
float bytes_to_float(unsigned char * bytes);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#endif /* __FONCTIONS_H */

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/
