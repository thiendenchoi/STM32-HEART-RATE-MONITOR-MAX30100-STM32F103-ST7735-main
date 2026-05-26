
/*--Include-start-------------------------------------------------------------*/
#include "user_main.h"
#include "stm32f1xx_hal.h"
#include "ST7735.h"
#include "stm32f1xx_hal_dma.h"
#include "gui.h"
#include "stdio.h"
#include "blood.h"
#include "MAX30100.h"
#include "string.h"
#include "main.h"
/*Include end ----------------------------------------------------------------*/

/*Global Data Space ----------------------------------------------------------*/

extern UART_HandleTypeDef huart1;

void User_Setup(void)
{
	Lcd_Init();
	ST7735_FillScreen(BLACK);
	
	max30100_init();

	blood_Setup();
}

void User_Loop(void)
{
	blood_Loop();
}

void User_Interrupt(void)
{
	blood_Interrupt();
}
