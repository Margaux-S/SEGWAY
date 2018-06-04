/**
  ******************************************************************************
  * @file    Gyroscope.c  
  * @author  INSA Toulouse
  * @version V1.0 
  * @date    13 Juin 2017 
  * @brief    
  ******************************************************************************
*/
/*Includes ------------------------------------------------------------------*/
#include "Gyroscope.h"

/**
  * @brief  Configure the L3GD2 to gyroscope application.
  * @param  Sensibilité ±250/±500/ ±2000 dps SPI 
  * @param  Digital output data rate (ODR) 95/190/380/760
  */
void Gyro_config(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;
  
  /* Configure Mems L3GD20 Bypass mode*/
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;// activation du circuit
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;// ODR = 95 190 390 760Hz ici  95Hz pour DATARATE_1 
	//L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_2;// ODR = 95 190 390 760Hz ici  190Hz pour DATARATE_2
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;// axes X, Y et Z activés
	//L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;// dépend du choix ODR precedent, le output data rate 95Hz et LPF 25 hz de coupure
	
	// Si choix precedent  est L3GD20_BANDWIDTH_1 --> 12.5 Hz de coupure
	L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_4;// dépend du choix ODR precedent 190Hz  ici  LPF 70 hz de coupure
	
	// CTRL_REG4 (23h) (0: continuos update; 1: output registers not updated until MSb and LSb reading)
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Continous;// continous
	
	//Big/little endian data selection. (0: Data LSb @ lower address; 1: Data MSb @ lower address)
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;//0--> Data LSb @ lower address
	
	//Full scale selection. Default value: 00 (00: 250 dps; 01: 500 dps; 10: 2000 dps; 11: 2000 dps)
  L3GD20_InitStructure.Full_Scale = L3GD20_SCALE; //500
  L3GD20_Init(&L3GD20_InitStructure);
  
	//High-pass filter mode 
  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
	
	//High-pass filter cut off frequency dépend de ODR
	//L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;//7.2 Hz pour ODR de 95Hz
	L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;//13.5 Hz pour ODR de 190Hz
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;
  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
  
	// Data ready	configuration de INT2
  L3GD20_INT2InterruptCmd(L3GD20_INT2INTERRUPT_ENABLE );// Activation de INT2 sortie du L3GD20
}


/**
  * @brief  Calculate the angular Data rate Gyroscope.
  * @param  pfData : Data out pointer
  * @retval None
  */
void Gyro_read (float* pfData)
{
  uint8_t tmpbuffer[6] ={0};
  int16_t RawData[3] = {0};
	// uint8_t tmpreg = 0;
  float sensitivity = 0;
  int i =0;

	//L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);
  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);
  RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);//ok
	sensitivity = L3G_Sensitivity;
  /* divide by sensitivity */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)RawData[i]/sensitivity;
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  return 0;
}

/************************ (C) COPYRIGHT INSA TOULOUSE ************************/
