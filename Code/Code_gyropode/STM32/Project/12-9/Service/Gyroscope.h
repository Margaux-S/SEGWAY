/**
  ******************************************************************************
  * @file    Gyroscope.h 
  * @author  INSA Toulouse
  * @version V1.0
  * @date    13-Juin-2017
  * @brief   Header for Gyroscope.c
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYROSCOPE_H
#define __GYROSCOPE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f3_discovery_l3gd20.h"

/* Private define ------------------------------------------------------------*/
#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	        /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps]*/

	// On a choisi sensitivité de 5000 dps 
#define L3G_Sensitivity L3G_Sensitivity_500dps 
#define L3GD20_SCALE     L3GD20_FULLSCALE_500 

/* Exported functions ------------------------------------------------------- */
void Gyro_config(void);
void Gyro_read(float* pfData);
uint32_t L3GD20_TIMEOUT_UserCallback(void);

#endif /* __GYROSCOPE_H */

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/
