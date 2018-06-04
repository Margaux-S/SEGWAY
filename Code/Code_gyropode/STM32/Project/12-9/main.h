/**
  ******************************************************************************
  * @file    main.h 
  * @author  INSA Toulouse
  * @version V1.0
  * @date    13-Juin-2017
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "Fonctions.h"

/* Global vairable ---------------------------------------------------------*/
extern int ConfigOK;

/* Exported functions ------------------------------------------------------- */
int main(void);
void assert_failed(uint8_t* file, uint32_t line);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

