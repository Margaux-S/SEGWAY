/**
  ******************************************************************************
  * @file    stm32f0xx_lcd.h
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
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0XX_LCD_H
#define __STM32F0XX_LCD_H
// #include "stm32f0xx.h"
// #include "stm32f0xx_delay.h"
#include "main.h"




/***********************************************************************************************
***********************************************************************************************/
/**************************************************************************
__Adaptation des lignes de l'afficheur au matériel mode 4 bit de donnees__
__Choix  du PortX et des pins_X__
*/

#define LCD_PORT GPIOD  // définir GPIOX
#define LCD_RS GPIO_Pin_0 // définir GPIO_Pin_X
#define LCD_RW GPIO_Pin_1 // définir GPIO_Pin_X
/* 
__Enable pulse durée état haut ~ 250ns min__Période 500 ns min
__Enregistre à l'état haut donnée présente et
__transfére sur front descendant donné présente 5 ns après__
*/
#define LCD_E GPIO_Pin_2 // définir GPIO_Pin_X

#define LCD_DB4 GPIO_Pin_3// définir GPIO_Pin_X
#define LCD_DB5 GPIO_Pin_4// définir GPIO_Pin_X
#define LCD_DB6 GPIO_Pin_5// définir GPIO_Pin_X
#define LCD_DB7 GPIO_Pin_6// définir GPIO_Pin_X

/*
__Taille caractère  0 pour 5 X8  Font F=0
*/

/*
__Nombre de lignes__
*/
#define	LCD_1_LIGNE 0 // ne pas modifier
#define	LCD_2_LIGNE 8 // ne pas modifier
#define	LCD_LIGNE 	LCD_2_LIGNE  // définir 	LCD_X_LIGNE  x = 1 ou 2
// Adresse ligne 1 : 0x80 et ligne 2 : 0xc0 ou (si une ligne => mode 2 lignes par fabrication) 
/*
__Ne pas modifier__
*/
#define LCD_LIGNES_UTILES LCD_RS | LCD_RW | LCD_E | LCD_DB7| LCD_DB6| LCD_DB5 | LCD_DB4
// commandes format 8 bits datasheet
#define LCD_OFF 0x0008  // afficheur off
#define	LCD_CLEAR	0x0001 // efface l'afficheur 1.6 ms d'éxecution
/*
* Entry mode set :0b000 00 00 0000 0 1 I/D SH
	   I/D = 0 écriture  de droite à gauche compteur -1	
     I/D = 1 écriture  de gauche à droite  compteur +1
		 SH = 0 écriture fixe normale
		 SH = 1 scrolling ( défilement ) à  gauche si I/D =1 à partir de l'adresse de départ 
     fixe à mettre le plus à gauche ---S  --SA   -SAL, si départ à 0 on ne voit rien !!!
     scrolling à droite si I/D = 0
		 */
#define LCD_ENTRY 0x0006
/*
* Display on/off : control 0b0000 0000 0000 1DCB
	    D = 1 afficheur on D = 0 afficheur off
	    C = 1 curseur on C = 0 cursseur off "SALUTcurseur     "
			B = 1 blink on b = 0 blink off
*/
#define LCD_ON  0x000c	// 
#define LCD_AD1  0x00  // définir  adresse DDRAM debut de chaine 1 ou de ligne 1 à définir
#define LCD_AD2  0x40  // définir adresse  DDRAM début de chaine 2 ou de ligne 2 à définir
/***********************************************************************************************
***********************************************************************************************/


void LCD_E_PULSE(void);	
void LCDsendCommand(uint16_t cmd); 
void LCDinit(void);
void LCDstring( const unsigned char *LCD_chaine, uint8_t adresse);   //Outputs string to LCD
unsigned char *bintoascii(uint16_t data_bin);
#endif /* __STM32F0XX_lcd_H */
/*
	**************************************************************************************
*/

