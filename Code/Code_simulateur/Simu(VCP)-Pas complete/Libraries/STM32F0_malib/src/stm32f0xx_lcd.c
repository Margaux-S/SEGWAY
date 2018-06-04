 /**
  ******************************************************************************
  * @file    stm32f0xx_lcd.c
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
#include "stm32f0xx_lcd.h"
  

//uint8_t etat_mesure = 0 ;
unsigned char T[6]="";
 unsigned char *chain_ascii = T;




 
/*********************************************************************************
**--------------------------------------------------------------------------------- 
  Format envoi la commande LCD 16 bits : X X X X_X X RS RW_DB7 DB6 DB5 DB4_DB3 DB2 DB1 DB0 
	et génération de E__ seule l'écriture est partiellement documentée
	* clear display	 0X0001  prévoir 1.6 ms d'exécution
  * return home	   0X0002  prévoir 1.6 ms d'exécution 
  * Entry mode set :000 00 00 0000 0 1 I/D SH
	   I/D = 0 écriture  de droite à gauche compteur +1	
     I/D = 1 écriture  de gauche à droite  compteur -1
		 SH = 1 scrolling ( défilement ) à  gauche si I/D =0 à partir de l'adresse de départ 
     fixe à mettre le plus à gauche ---S  --SA   -SAL, si départ à 0 on ne voit rien !!!
     scrolling à deoite si I/D = 1
	* Display on/off : control 0b0000 0000 0000 1DCB
	    D = 1 afficheur on D = 0 afficheur off
	    C = 1 curseur on C = 0 cursseur off
			B = 1 blink on b = 0 blink off
  *	Set DDRAM adress 0b0000 0000 1xxx xxxx mode 1 ou  2 lignes
	    xxx xxxx 7 bits ->0x00 à 0x4F une ligne 
			xxx xxxx 7 bits ->0x00 à 0x27 ligne 1 et 0X40 à 0X67 ligne 2
	*	Ecriture  DDRAM  RS = 1 RW =0 0b0000 0010 xxxx xxxx  
	  8 bits
	---------------------------------------------------------------------------------- 
*/


	void LCDsendCommand(uint16_t cmd)	
{
	uint16_t PortVal;// variable d'acces au port de commabde
	PortVal = 0;

		
	GPIO_ResetBits(LCD_PORT,LCD_LIGNES_UTILES);// mise à 0 data LCD port
	/*
__ DB7 DB6 DB5 DB4  alignées sur PA6 à PA3__
__ RS RW réalignées sur PA0 et PA1__
	*/
	if (cmd & 0x0200) (PortVal |=  LCD_RS);//PA0
	if (cmd & 0x0100) (PortVal |=  LCD_RW);//PA1
	if (cmd & 0x0010) (PortVal |=  LCD_DB4);//PA3
	if (cmd & 0x0020) (PortVal |=  LCD_DB5);//PA4
	if (cmd & 0x0040) (PortVal |=  LCD_DB6);//PA5
	if (cmd & 0x0080) (PortVal |=  LCD_DB7);//PA6
  PortVal  |= (GPIO_ReadOutputData( LCD_PORT));
	GPIO_Write(LCD_PORT, PortVal);
   LCD_E_PULSE();		// pulse E
	PortVal = 0;
	GPIO_ResetBits(LCD_PORT,LCD_LIGNES_UTILES);// mise à 0 data LCD port
	/*
__DB3 DB2 DB1 DB0 aligneées sur PA6 à PA3__
__RS RW réalignées sur PA0 et PA1__
	*/
  if (cmd & 0x0200) (PortVal |=  LCD_RS);//PA0
	if (cmd & 0x0100) (PortVal |=  LCD_RW);//PA1
	if (cmd & 0x0001) (PortVal |=  LCD_DB4);//PA3
	if (cmd & 0x0002) (PortVal |=  LCD_DB5);//PA4
	if (cmd & 0x0004) (PortVal |=  LCD_DB6);//PA5
	if (cmd & 0x0008) (PortVal |=  LCD_DB7);//PA6
  PortVal  |= GPIO_ReadOutputData( LCD_PORT);
	GPIO_Write(LCD_PORT, PortVal);
	LCD_E_PULSE();// pulse E	
		
}

 
/*******************************************************************************************************
**--------------------------------------------------------------------------------- 
  Initialisation du LCD
	Procédure obligatoire
	 
	---------------------------------------------------------------------------------- 
*/


	void LCDinit(void)	
{
/*
Initialisation obligatoire
*/

GPIO_ResetBits(LCD_PORT,LCD_LIGNES_UTILES);// mise à 0 data port
Delay(15000);// LCD Delais de 15 ms 1500
GPIO_SetBits(LCD_PORT,LCD_DB5 | LCD_DB4);
LCD_E_PULSE();
Delay(500);// LCD Delais de 5 ms 
GPIO_SetBits(LCD_PORT,LCD_DB5 | LCD_DB4);
LCD_E_PULSE();
Delay(100);// LCD Delais de 1 ms  
GPIO_SetBits(LCD_PORT,LCD_DB5 | LCD_DB4);
LCD_E_PULSE();
Delay(100);// LCD Delais de 1 ms 

	/*
__Mode 4 bits__A faire ici seulement
*/
GPIO_ResetBits(LCD_PORT,LCD_DB4); 
GPIO_SetBits(LCD_PORT,LCD_DB5 );  
LCD_E_PULSE();

/*
__LIGNE  Font normale par defaut__
*/

LCDsendCommand(0x0020 |LCD_LIGNE );//0x0028
/*
__Affichage commandes démarrage
*/

LCDsendCommand(LCD_OFF);
LCDsendCommand(LCD_CLEAR);
Delay(200);// LCD Delais de 2 ms 
LCDsendCommand(LCD_ENTRY);
LCDsendCommand(LCD_ON);

}




 
/*********************************************************************************
 *
	--------------------------------------------------------------------------------- 
  Génération du pulse de validation de la data
	* 
 	---------------------------------------------------------------------------------- 
*/


void LCD_E_PULSE(void)	
{
/* Mise à 1 de E */
	Delay(2);
	GPIO_SetBits(LCD_PORT,(LCD_E));
	Delay(1);//0.1 ms 10
	/* Mise à 0 de E */
  GPIO_ResetBits(LCD_PORT,( LCD_E ));// mise à 0 de E 
			Delay(1);//0.1 ms	10
}


 
/*********************************************************************************
  *
	--------------------------------------------------------------------------------- 
  Envoie de la chaine à afficher avec l'adresse de sa  position 
  Continuité automatique de lécriture de la ligne 1 à 2
  Exemple cas du un ligne, 2 X 8 caratères :
	0x80 pour les 8 premiers caractéres et 0xc0 pour les 8 suivants
	---------------------------------------------------------------------------------- 
*/


void LCDstring( const unsigned char *LCD_chaine, uint8_t adresse)   //Outputs string to LCD
{
//uint8_t id; 
LCDsendCommand (adresse);
while (*LCD_chaine) {
		LCDsendCommand((*LCD_chaine| 0x0200));
	  LCD_chaine++;
//		id++;
//	if ((id + adresse ) == 0x0088) LCDsendCommand(0x00C0);

	}
}

/*********************************************************************************
 *
	--------------------------------------------------------------------------------- 
  Conversion mot non signé de 16 bits en une chaine de caractère
  Max 65535
	---------------------------------------------------------------------------------- 
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






