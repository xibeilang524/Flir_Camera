/*-----------------------------------------------------------------
 * Name:    temprature.h  
 * Purpose: stm32f411 internal_temprature driver include  
 *-----------------------------------------------------------------
 * 
 * Copyright (c) *reserve
 
||                       _      _               ||
||    /\  /\  __  _  __ | | __ | | ____  ___ _  ||
||   /  \/  \/ _ ` |/ _ ` |/ _ ` |/ _  \/ _ ` | ||
||  / /\  /\  (_|    (_|    (_|    (_)   (_)  | ||
|| /_/  \/  \_.__, |\__, _|\__, _|\____/\___. | ||
|| =====================================|____/  ||
||                                              ||

 -----------------------------------------------------------------*/
 
/********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************/
#include "stm32f4xx_hal.h"
#include "temprature.h"

#include "stdint.h"
#include "flir_lcd.h"
#include "flir_menu.h"
#include "key.h"
#include "electricity.h"
#include "stmflash.h"

/********************************************************************************************************
 *                                                 MACROS
 ********************************************************************************************************/
const float Vref = 3.0425;                // 新版
//const float Vref = 3.3395;              // 旧版
/********************************************************************************************************
 *                                               CONSTANTS
 ********************************************************************************************************/
 
const uint8_t temp_number[10][10] = 
{
	{ 254, 198, 146, 146, 146, 146, 146, 146, 198, 254},       // 0
	{ 254, 230, 198, 134, 166, 230, 230, 230, 230, 254},       // 1
	{ 254, 198, 146, 242, 242, 230, 206, 158, 130, 254},       // 2
	{ 254, 198, 146, 242, 230, 242, 242, 146, 198, 254},       // 3
	{ 254, 242, 226, 210, 210, 178, 128, 242, 242, 254},       // 4
	{ 254, 194, 206, 158, 134, 178, 242, 146, 198, 254},       // 5
	{ 254, 198, 146, 158, 134, 146, 146, 146, 198, 254},       // 6
	{ 254, 130, 242, 230, 230, 206, 206, 206, 206, 254},       // 7
	{ 254, 198, 146, 146, 198, 146, 146, 146, 198, 254},       // 8
	{ 254, 198, 146, 146, 146, 194, 242, 146, 198, 254}        // 9
};
 
/********************************************************************************************************
 *                                               GLOBAL VARIABLES
 ********************************************************************************************************/

/********************************************************************************************************
 *                                               EXTERNAL VARIABLES
 ************************************ ********************************************************************/
extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef LCD_DMA_PORT;

extern uint16_t rowBuf[FLIR_LCD_RAW][FLIR_LCD_COLUMNUM];
/********************************************************************************************************
 *                                               EXTERNAL FUNCTIONS
 ********************************************************************************************************/


/********************************************************************************************************
 *                                               LOCAL VARIABLES
 ********************************************************************************************************/

 
/********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************/

 
/********************************************************************************************************
 *                                               PUBLIC FUNCTIONS
 ********************************************************************************************************/
 
/*********************************************************************
 * @fn        Get_Adc()
 *
 * @brief     获得某个通道ADC的值  
 *
 * @param     ch：ADC_Channel 
 *
 * @return    ADC寄存器值
 */
uint16_t  Get_Adc(uint32_t ch)
{
	uint16_t ADC_Temp = 0;
	ADC_ChannelConfTypeDef ADC1_ChanConf;
		
	ADC1_ChanConf.Channel=ch;                                   // 通道
	ADC1_ChanConf.Rank=1;                                       // 1个序列
	ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_3CYCLES;          // 采样时间
	ADC1_ChanConf.Offset=0;                 
	HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);               // 通道配置

	HAL_ADC_Start(&hadc1);                                      // 开启ADC
 	if(HAL_ADC_PollForConversion(&hadc1,2) == HAL_OK)
	{
		 ADC_Temp = HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);
	return ADC_Temp;	            //返回最近一次ADC1规则组的转换结果
}

/*********************************************************************
 * @fn        Get_Adc_Average()
 *
 * @brief     得到某个通道给定次数采样的平均值
 *
 * @param     ch：ADC_Channel     times:采样次数
 *
 * @return    给定次数采样的平均值
 */
uint16_t Get_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	
	for(t=0;t<times;t++)
	{
		temp_val += Get_Adc(ch);
	}
	return temp_val / times;
}

/*********************************************************************
 * @fn        Get_Temprate()
 *
 * @brief     获取STM32F411内部温度传感器的温度值
 *
 * @param     None  
 *
 * @return    STM32F411内部温度传感器的温度值
 */
short Get_Temprate(void)
{
 	uint32_t adcx;
 	double temperate = 0;
	
	adcx=Get_Adc(ADC_CHANNEL_TEMPSENSOR);    //读取内部温度传感器通道
	temperate=(float)adcx*(Vref/4096);		   //电压值
	temperate=(temperate-0.76)/0.0025 + 25;  //转换为温度值 		
	
	return (float)temperate;
}

/*********************************************************************
 * @fn      display_Versionmenu
 *
 * @brief   display current temprature Message
 *
 * @param   none
 *
 * @return  
 */
void display_Temperature(float temp)
{
	uint16_t index;
	uint16_t i;
	uint8_t Temp_Value = 0;
	
	short temprature = (short)(temp * 1.80f + 32);    // 摄氏温度转华氏温度
	
	// 显示温度  
	Temp_Value = temprature / 100;      // 温度值    百位
	// block sending
	if(Temp_Value != 0)
	{
		for(index = 0; index < 10; index++)
		{
			for(i = 0; i < 7; i++)         // 数字的分辨率仅为10 x 7
			{
				if(((temp_number[Temp_Value][(index * (16/2) + i) >> 3] >> (7 - (index * (16/2) + i)%8)) & 0x01) != 0x01)
					rowBuf[index+3][1+i] = 0xE007;
			}
		}
	}
	Temp_Value = temprature / 10 % 10;   // 温度值    十位
	// block sending
	for(index = 0; index < 10; index++)
	{
		for(i = 0; i < 7; i++)          // 数字的分辨率仅为10 x 7
		{
			if(((temp_number[Temp_Value][(index * (16/2) + i) >> 3] >> (7 - (index * (16/2) + i)%8)) & 0x01) != 0x01)
				rowBuf[index+3][8+i] = 0xE007;
		}
	}
	
	Temp_Value = temprature % 10;        // 温度值    个位
	// block sending
	for(index = 0; index < 10; index++)
	{
		for(i = 0; i < 7; i++)             // 数字的分辨率仅为10 x 7
		{
			if(((temp_number[Temp_Value][(index * (16/2) + i) >> 3] >> (7 - (index * (16/2) + i)%8)) & 0x01) != 0x01)
				rowBuf[index+3][15+i]=0xE007;
		}
	}
}

 /********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************/

/*********************************************************************
 * @fn      
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 */
 
 


/*********************************************************************
 */
