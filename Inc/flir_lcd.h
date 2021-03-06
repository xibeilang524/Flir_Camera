/*-----------------------------------------------------------------
 * Name:      flir_lcd.h
 * Purpose:   work with flir_lcd.c
 *-----------------------------------------------------------------
 * Provide macro defination and function/variable extern for 
 * flir_lcd.c 
 *
 * The pin usage is:
 *
 * PB0 - RST
 * PB1 - RS
 * PB2 - CE
 * PA5 - SCK
 * PA7 - MOSI
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
 #ifndef FLIR_LCD_H_
 #define FLIR_LCD_H_
 
/********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************/
/* System ralted */
#include "stm32f4xx_hal.h"
#include <string.h> 	
#include "stdlib.h"
#include "flir_menu.h"
#include "sys.h"

#include <stdbool.h>

/********************************************************************************************************
 *                                                 MACROS
 ********************************************************************************************************/
// define the hardware used to connect LCD
#define LCD_SPI_PORT		hspi1
#define LCD_DMA_PORT		hdma_spi1_tx

/* LCD basic information */
#define LCD_RAW					128
#define LCD_COLUMN			160
#define LCD_PIXPOINT		20480

#define LCD_FULSIZE_BUF		40960

/* buffer size defination */
// clear buffer size
#define LCD_CLEAR_BUF_SIZ	320
#define LCD_CLEAR_ROUND		(LCD_FULSIZE_BUF/LCD_CLEAR_BUF_SIZ)


/* Control related*/
//LCD_RST
#define SPILCD_RST_SET		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET)  //PC14 
#define SPILCD_RST_RESET	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET)//PC14 

//LCD_RS//dc  
#define SPILCD_RS_SET			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET)  //PC15
#define SPILCD_RS_RESET		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET)//PC15 

#ifndef FLIR_PROJ
//LCD_CS
#define SPILCD_CS_SET 	 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)  //PA4
#define SPILCD_CS_RESET  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)//PA4
#endif

//LCD REST    
#define	LCD_REST PCout(14) 			

/* Color define */
#define WHITE				0xFFFF
#define BLACK				0x0000	  
#define BLUE				0x001F  
#define BRED				0XF81F
#define GRED 				0XFFE0
#define GBLUE				0X07FF
#define RED       	0xF800
#define MAGENTA   	0xF81F
#define GREEN      	0x07E0
#define CYAN       	0x7FFF
#define YELLOW     	0xFFE0
#define BROWN 			0XBC40
#define BRRED 			0XFC07
#define GRAY  			0X8430 


/* Type Define */
typedef struct  
{ 					    
	uint16_t width;			// LCD width
	uint16_t height;		// LCD height
	uint16_t id;				// LCD ID
	uint8_t	wramcmd;		// write commmand
	uint8_t  setxcmd;		// set x position command
	uint8_t  setycmd;		// set y position command
}_lcd_dev; 


#ifdef FLIR_PROJ
/* Flir Related */
#define FLIR_LCD_PIX	19200 
#define FLIR_PIXPOINT		4800	// 80*60 point

#define FLIR_PACKLINELEN	164 // 164 bytes length

#define FLIR_LCD_RAW					120

#define FLIR_LCD_COLUMNUM			160
#define Compass_COLUMNUM			 16

#define FLIR_PACKLINELEN_16BIT		82	// 164 bytes per package, 82 16bits 

#define FLIR_COLUMNUM		80	// 80 unit per line

#define FLIR_LINE				60	// 60 lines

#define FLIR_TEL				3	// flir tel line

#define FLIR_TELLINE		st(60 + FLIR_TEL)	// image line with tel line
#endif

/********************************************************************************************************
 *                                               CONSTANTS
 ********************************************************************************************************/



/********************************************************************************************************
 *                                               EXTERNAL VARIABLES
 ********************************************************************************************************/
// LCD configuration variable
extern _lcd_dev lcddev;	
extern volatile bool lcdTXcpl;
extern uint16_t rowBuf[FLIR_LCD_RAW][FLIR_LCD_COLUMNUM];

/********************************************************************************************************
 *                                               EXTERNAL FUNCTIONS
 ********************************************************************************************************/
/*********************************************************************
 * @fn      LCD_SetCursor
 *
 * @brief   Set cursor position. The size is 160*128.
 *          And the top left is the starting address, (0,0)
 *
 *			X axis is the long side, Y is the short side.
 *
 * @param   uint16_t Xpos - X position. 
 *			uint16_t Ypos - Y position.
 *
 * @return  none
 */
extern void LCD_SetCursor(uint16_t , uint16_t );

/*********************************************************************
 * @fn      LCD_DrawPoint
 *
 * @brief   Draw a single point with x.y position provide.
 *
 *			X axis is the long side, Y is the short side.
 *
 * @param   uint16_t x - X position. 
 *					uint16_t y - Y position.
 *					uint16_t color - point color
 * @return  none
 */
extern void LCD_DrawPoint(uint16_t ,uint16_t, uint16_t );

/*********************************************************************
 * @fn      LCD_Init
 *
 * @brief   Init LCD. Set basic information about the LCD
 *
 *			- Set color mode 65k
 *			- Revert X,Y axis
 *			- Set Frame Rate and power settings.
 *
 * @param   none
 *
 * @return  none
 */
extern void LCD_Init(void);

/*********************************************************************
 * @fn      LCD_Clear
 *
 * @brief   Clear LCD. Set screen to sigle color. 
 *
 * @param   n
 *
 * @return  none
 */
extern void LCD_Clear(uint16_t);

/*********************************************************************
 * @fn      LCD_WriteRAM_Prepare
 *
 * @brief   Prepare to send graphic information to GRam.
 *			The graphic data should be transmit after this command. 
 *
 * @param   none
 *
 * @return  none
 */
extern void LCD_WriteRAM_Prepare(void);

#ifdef FLIR_PROJ
/*********************************************************************
 * @fn      LCD_WR_Frame
 *
 * @brief   Display a whole frame data
 *
 * @param   uint16_t * pdata - data pointer
 *
 * @return  transmit status
 */
bool LCD_WR_Frame(volatile uint16_t [][80]);

/*********************************************************************
 * @fn      LCD_drawLine
 *
 * @brief   draw a line 
 *
 * @param   uint16_t x1	- point1 x location
 *					uint16_t y1 - point1 y location
 *					uint16_t x2 - point2 x location
 *					uint16_t y2 - point2 y location
 *					uint16_t color - line color
 *					
 * @return  none
 */
void LCD_drawLine(uint16_t , uint16_t , uint16_t , uint16_t , uint16_t );

void SET_BGLight(BrightnessCont_sta levels);
	
#endif

#endif
/*********************************************************************
 */

