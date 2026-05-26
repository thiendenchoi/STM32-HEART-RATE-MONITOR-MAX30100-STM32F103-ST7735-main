/*--Include-start-------------------------------------------------------------*/
#include "blood.h"
#include "MAX30100.h"
#include "algorithm.h"
#include "math.h"
#include "gui.h"
#include "stdio.h"
#include "ST7735.h"
#include "beep.h"
#include "string.h"
#include "main.h"

/*Global data space ----------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

uint16_t g_fft_index = 0;

struct compx s1[FFT_N+16];
struct compx s2[FFT_N+16];

struct
{
	float 	Hp	;
	float 	HpO2;

}g_BloodWave;

BloodData g_blooddata = {0};

#define CORRECTED_VALUE			47

/* Private function prototypes */
void tft_draw_windows(void);
void tft_draw_frame(uint16_t color);
void tft_draw_State(uint8_t st);
void tft_draw_wave_line(uint8_t p,uint8_t n,uint16_t h0,uint16_t h1,uint16_t color);
void tft_draw_wave(void);
void tft_draw_hrsp(void);


/*funcation start ------------------------------------------------------------*/


/*Sensor func -----------------------------------------------------------------*/
void blood_data_update(void)
{
	uint16_t temp_num=0;
	uint16_t fifo_word_buff[1][2];

	temp_num = max30100_Bus_Read(INTERRUPT_REG);

	if (INTERRUPT_REG_A_FULL&temp_num)
	{
		max30100_FIFO_Read(0x05,fifo_word_buff,1);

		for(int i = 0;i < 1;i++)
		{
			if(g_fft_index < FFT_N)
			{
				s1[g_fft_index].real = fifo_word_buff[i][0];
				s1[g_fft_index].imag= 0;
				s2[g_fft_index].real = fifo_word_buff[i][1];
				s2[g_fft_index].imag= 0;
				g_fft_index++;
			}
		}
		g_blooddata.update++;
	}
	else
	{
		asm("nop");
	}
}

void blood_data_translate(void)
{
	if(g_fft_index>=FFT_N)
	{
		Gui_DrawFont_GBK16(102,2,BLACK,GREEN,"FFT");

		g_fft_index = 0;

		g_blooddata.display = 1;

		FFT(s1);
		FFT(s2);

		for(int i = 0;i < FFT_N;i++)
		{
			s1[i].real=sqrtf(s1[i].real*s1[i].real+s1[i].imag*s1[i].imag);
			s2[i].real=sqrtf(s2[i].real*s2[i].real+s2[i].imag*s2[i].imag);
		}

		uint16_t s1_max_index = find_max_num_index(s1, 60);
		uint16_t s2_max_index = find_max_num_index(s2, 60);

		if(abs(s1_max_index - s2_max_index) <= 2)
		{
			uint16_t Heart_Rate = 60 * SAMPLES_PER_SECOND * s2_max_index / FFT_N;

			g_blooddata.heart = Heart_Rate - 10;

			float sp02_num = (s2[s1_max_index].real * s1[0].real)
											/(s1[s1_max_index].real * s2[0].real);

			sp02_num = (1 - sp02_num) * SAMPLES_PER_SECOND + CORRECTED_VALUE;

			g_blooddata.SpO2 = sp02_num;

			g_blooddata.state = BLD_NORMAL;
		}
		else
		{
			g_blooddata.heart = 0;
			g_blooddata.SpO2 	= 0;
			g_blooddata.state = BLD_ERROR;
		}
		Gui_DrawFont_GBK16(102,2,GREEN,BLACK,"FFT");
	}
}

void blood_wave_update(void)
{
	static DC_FilterData hbdc = {.w = 0,.init = 0,.a = 0};
	static DC_FilterData hbodc = {.w = 0,.init = 0,.a = 0};

	float hbag = 0;
	float hboag = 0;

	float hbag_d = 0;
	float hboag_d = 0;

	for(int i = 0;i < 8;i++)
	{
		hbag  += s1[g_fft_index - 8 + i].real;
		hboag += s2[g_fft_index - 8 + i].real;
	}

	hbag_d = dc_filter(hbag,&hbdc) / 8;
	hboag_d = dc_filter(hboag,&hbodc) / 8;

	float hbhight  = 0;
	float hbohight = 0;

	hbhight  = (-hbag_d / 40.0) + 5;
	hbohight  = (-hboag_d / 40.0) + 5;

	hbhight = (hbhight > 27) ? 27 : hbhight;
	hbhight = (hbhight < 0) ?  0 : hbhight;

	hbohight = (hbohight > 27) ? 27 : hbohight;
	hbohight = (hbohight < 0) ?  0 : hbohight;

	g_BloodWave.Hp = hbhight;
	g_BloodWave.HpO2 = hbohight;

	if(hbag < 5000 ||hbag < 5000)
	{
		g_blooddata.heart = 0;
		g_blooddata.SpO2 	= 0;

		g_fft_index -= 7;

		if(g_blooddata.state != BLD_ERROR)
		{
			g_blooddata.state = BLD_ERROR;
			g_blooddata.display = 1;
		}
	}
	else
	{
		if(g_blooddata.state == BLD_ERROR)
		{
			g_blooddata.state = BLD_NORMAL;
			g_blooddata.display = 1;
		}
	}
}
/*tft display ---------------------------------------------------------------*/

void tft_test_display(void)
{
	uint8_t str[50];

	if (g_blooddata.display == 1)
	{
		g_blooddata.display = 0;

		sprintf((char *)str,"heart = %3d",g_blooddata.heart);
		Gui_DrawFont_GBK16(8,8,0x00FF,BLACK,str);

		Gui_DrawFont_GBK16(8,26,0x00FF,BLACK,str);

		if(g_blooddata.state)
		{
			sprintf((char *)str,"ERROR     ");
			Gui_DrawFont_GBK16(8,44,0xF000,BLACK,str);
		}
		else
		{
			sprintf((char *)str,"NORMAL    ");
			Gui_DrawFont_GBK16(8,44,0x07E0,BLACK,str);
		}
	}
}

void tft_draw_frame(uint16_t color)
{
	gui_draw_square(0,1,127,127,color);
	gui_draw_square(0,1,127,18,color);
	gui_draw_square(0,1,100,18,color);
	gui_draw_square(0,57,127,127,color);
}

void tft_draw_State(uint8_t st)
{
	switch(st)
	{
		case BLD_NORMAL:
		{
			tft_draw_frame(GRAY1);
			Gui_DrawFont_GBK16(3,2,0x001F,GRAY3,"Normal    ");
			break;
		}
		case BLD_ERROR:
		{
			tft_draw_frame(RED);
			Gui_DrawFont_GBK16(3,2,0xF000,GRAY3,"Caution   ");
			break;
		}
	}
}

void tft_draw_wave_line(uint8_t p,uint8_t n,uint16_t h0,uint16_t h1,uint16_t color)
{
	uint16_t x = 4;
	uint16_t y = (p == 0) ? 122 : 89;

	if((n + 1) < 82)
	{
		Gui_DrawLine((x + n + 2),y - 28,(x + n + 2),y + 1,GRAY2);
		Gui_DrawLine((x + n + 1),y - 28,(x + n + 1),y + 1,BLACK);
	}
	Gui_DrawLine((x + n),(y - h0),(x + n + 1),(y - h1),color);

}

void tft_draw_wave(void)
{
	static uint16_t hist_n  = 0;

	static uint16_t hbhist_h  = 0;
	static uint16_t hbohist_h = 0;

	tft_draw_wave_line(1, hist_n, hbhist_h , g_BloodWave.Hp, 0x8FF0);
	tft_draw_wave_line(0, hist_n, hbohist_h, g_BloodWave.HpO2, 0xFC1F);

	hist_n = (hist_n < 81) ? (hist_n + 1) : 0;

	hbhist_h = g_BloodWave.Hp;
	hbohist_h = g_BloodWave.HpO2;
}

void tft_draw_hrsp(void)
{
	uint8_t str[50];

	sprintf((char *)str,"%3d  ",g_blooddata.heart);
	Gui_DrawFont_GBK16(40,20,0xffe0,GRAY3,str);
	g_blooddata.SpO2 = (g_blooddata.SpO2 > 99.99) ? 99.99:g_blooddata.SpO2;
	sprintf((char *)str," %2.2f%%  ",g_blooddata.SpO2);
	Gui_DrawFont_GBK16(40,38,0x07ff,GRAY3,str);
}


void tft_draw_windows(void)
{
	Lcd_Clear(GRAY3);

	tft_draw_frame(GRAY1);


	gui_draw_Block(3,60,87,91,GRAY2,BLACK);
	gui_draw_Block(3,93,87,124,GRAY2,BLACK);

	gui_draw_Block(89,60,125,91,GRAY2,BLACK);
	gui_draw_Block(89,93,125,124,GRAY2,BLACK);

	Gui_DrawFont_GBK16(3,20,0xffe0,GRAY3,"HR  :");
	Gui_DrawFont_GBK16(3,38,0x07ff,GRAY3,"SpO2:");

	Gui_DrawFont_GBK16(91,67,0x8FF0,BLACK," Hb");
	Gui_DrawFont_GBK16(91,100,0xFC1F,BLACK,"HbO2");

	Gui_DrawFont_GBK16(102,2,GREEN,BLACK,"FFT");

	Gui_DrawFont_GBK16(3,2,0x001F,GRAY3,"Normal    ");
}

void tft_display_update(void)
{
	if(g_blooddata.update >= 8)
	{
		g_blooddata.update = 0;
		blood_wave_update();
		tft_draw_wave();
	}

	if(g_blooddata.display >= 1)
	{
		if (g_blooddata.state == BLD_NORMAL && g_blooddata.SpO2 <= 100.0)
		{
			char tx_buff[64];

			sprintf(tx_buff, "HR: %d, SpO2: %.2f, State: %d\r\n",
			        g_blooddata.heart, g_blooddata.SpO2, g_blooddata.state);

			HAL_UART_Transmit(&huart1, (uint8_t*)tx_buff, strlen(tx_buff), 100);
		}

		g_blooddata.display = 0;
		tft_draw_State(g_blooddata.state);
		tft_draw_hrsp();
	}
}

void Led_beep_update(void)
{
	static uint32_t errorStartTick = 0;
	static BloodState blhist = BLD_NORMAL;
	static uint8_t beepstate = 0;


	switch(g_blooddata.state)
	{
		case BLD_NORMAL:
		{
			asm("nop");
			break;
		}
		case BLD_ERROR:
		{
			if(blhist == BLD_NORMAL)
			{
				errorStartTick = HAL_GetTick();
			}

			if((HAL_GetTick() - errorStartTick) < 3000)
			{
				if(( (HAL_GetTick() / 100) %
						 (4-(HAL_GetTick() - errorStartTick) / 1000)
				   ) == 0)
				{

					asm("nop");
				}
				else
				{

					asm("nop");
				}

			}
			else if(3000 < (HAL_GetTick() - errorStartTick) &&
							6000 > (HAL_GetTick() - errorStartTick))
			{

				asm("nop");
			}
			else if((HAL_GetTick() - errorStartTick) > 6000)
			{

				asm("nop");
			}
			break;
		}
		default:break;
	}

	blhist = g_blooddata.state;
}


/*Setup and loop func -----------------------------------------------------*/

void blood_Setup(void)
{
	g_blooddata.heart = 0;
	g_blooddata.SpO2 = 0;
	g_blooddata.display = 1;

	tft_draw_windows();

}

void blood_Loop(void)
{
	blood_data_update();
	blood_data_translate();
	tft_display_update();
	Led_beep_update();

}

void blood_Interrupt(void)
{
	static int16_t div = 0;
	static int16_t div2 = 0;
	div++;
	if(div > 10)
	{
		div = 0;
	}

	div2++;
	if(div > 50)
	{
		div2 = 0;
	}
}
