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
__Adaptation des lignes de l'afficheur au mat�riel mode 4 bit de donnees__
__Choix  du PortX et des pins_X__
*/

#define LCD_PORT GPIOD  // d�finir GPIOX
#define LCD_RS GPIO_Pin_0 // d�finir GPIO_Pin_X
#define LCD_RW GPIO_Pin_1 // d�finir GPIO_Pin_X
/* 
__Enable pulse dur�e �tat haut ~ 250ns min__P�riode 500 ns min
__Enregistre � l'�tat haut donn�e pr�sente et
__transf�re sur front descendant donn� pr�sente 5 ns apr�s__
*/
#define LCD_E GPIO_Pin_2 // d�finir GPIO_Pin_X

#define LCD_DB4 GPIO_Pin_3// d�finir GPIO_Pin_X
#define LCD_DB5 GPIO_Pin_4// d�finir GPIO_Pin_X
#define LCD_DB6 GPIO_Pin_5// d�finir GPIO_Pin_X
#define LCD_DB7 GPIO_Pin_6// d�finir GPIO_Pin_X

/*
__Taille caract�re  0 pour 5 X8  Font F=0
*/

/*
__Nombre de lignes__
*/
#define	LCD_1_LIGNE 0 // ne pas modifier
#define	LCD_2_LIGNE 8 // ne pas modifier
#define	LCD_LIGNE 	LCD_2_LIGNE  // d�finir 	LCD_X_LIGNE  x = 1 ou 2
// Adresse ligne 1 : 0x80 et ligne 2 : 0xc0 ou (si une ligne => mode 2 lignes par fabrication) 
/*
__Ne pas modifier__
*/
#define LCD_LIGNES_UTILES LCD_RS | LCD_RW | LCD_E | LCD_DB7| LCD_DB6| LCD_DB5 | LCD_DB4
// commandes format 8 bits datasheet
#define LCD_OFF 0x0008  // afficheur off
#define	LCD_CLEAR	0x0001 // efface l'afficheur 1.6 ms d'�xecution
/*
* Entry mode set :0b000 00 00 0000 0 1 I/D SH
	   I/D = 0 �criture  de droite � gauche compteur -1	
     I/D = 1 �criture  de gauche � droite  compteur +1
		 SH = 0 �criture fixe normale
		 SH = 1 scrolling ( d�filement ) �  gauche si I/D =1 � partir de l'adresse de d�part 
     fixe � mettre le plus � gauche ---S  --SA   -SAL, si d�part � 0 on ne voit rien !!!
     scrolling � droite si I/D = 0
		 */
#define LCD_ENTRY 0x0006
/*
* Display on/off : control 0b0000 0000 0000 1DCB
	    D = 1 afficheur on D = 0 afficheur off
	    C = 1 curseur on C = 0 cursseur off "SALUTcurseur     "
			B = 1 blink on b = 0 blink off
*/
#define LCD_ON  0x000c	// 
#define LCD_AD1  0x00  // d�finir  adresse DDRAM debut de chaine 1 ou de ligne 1 � d�finir
#define LCD_AD2  0x40  // d�finir adresse  DDRAM d�but de chaine 2 ou de ligne 2 � d�finir
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

