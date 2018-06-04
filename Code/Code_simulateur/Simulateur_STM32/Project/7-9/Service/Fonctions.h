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
#define PI       3.14159265f 
#define Deltatps 0.010526f   						// Delta t du Gyro  = 1/95 HZ 
#define ScaleADC 3000                   // pleine echelle ADC 3V 
#define Freq_SYS 72000000								// frequence systeme

// Constantes du système
#define R 0.47f							// Resistance de moteur 
#define L 0.00047f					// Inductance de bobine de moteur 
#define Km 0.1375f					// Constante de couple du moteur (C = Km * I) 
#define Kg 6.3636f
#define Rw 0.127f

/* Global variables ---------------------------------------------------------*/

//Variable de gestion de ADC
extern __IO uint32_t  ADC1ConvertedValue , ADC1ConvertedVoltage;
extern __IO uint16_t MOY_ADC12_IN8 , MOY_ADC12_IN9 ;;// variables lectures ADC moyennees de Acq_ADC1[2]

//Variable de gestion de capteur gyro-acc
extern float ZeroACCX ;								// variable de zero accelero X
extern float Zerogyro ;								// variable de zero du gyro
extern float Angleacc ;								// variable de l'angle a partir de tangente inverse
extern float AccBuffer[3], Buffer[3];	// tableau pour lecture de valeurs par gyroscope et accelerometre
extern float Anglegyro;								// variable de l'angle a partir de d °/seconde
extern float Anglemesure;							// variable de l'angle fusionné
extern float VitesseAngle;						// variable de la vitesse angulaire

// Variable for gestion de consigne 
extern float Consigne;								// variable de consigne reçu de Raspberry Pi 

//Variable pour fonction delay
extern __IO uint32_t TimingDelay ;

//Variable de mode de entree de beta 
extern int usage_acc;

//Variable pour fonction de arret trigger par Raspberry Pi
extern int arret;											//	variable de l'état arrêt urgence declecnhé par Raspberry Pi 
extern int cmpt_arr;									//	variable de compteur pour reinitialiser état de arret de système 

/* Private macro -------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x

/* Exported functions ------------------------------------------------------- */
void Trait_Gyro_Acc (void);
unsigned char * bintoascii(uint16_t data_bin);
float bytes_to_float(unsigned char * bytes);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#endif /* __FONCTIONS_H */

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/
