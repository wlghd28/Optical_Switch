/*
 * st7735.c
 *
 *  Created on: Jul 24, 2023
 *      Author: hojin
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "dma.h"
#include "spi.h"
#include "gpio.h"
#include "st7735.h"
#include "fonts.h"
#include "sw.h"
#include "hw.h"

uint8_t g_ST7735_SPI_TX_DMA = 0;		// 0 이면 SPI 송신 준비, 1 이면 SPI 송신 대기

//Send Command
void st7735_write_cmd(uint8_t cmd)
{
	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	SetCS(RESET);

	//Send Command
	SetDC(RESET);

	if(HAL_SPI_Transmit_IT(&hspi4,&cmd,1)!=HAL_OK)
	{

	}

	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	SetCS(SET);
}

//Send Command, Command Parameter, Parameter Size
void st7735_write_cmd_param(uint8_t cmd, uint8_t *parameter, uint8_t param_size)
{
	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	SetCS(RESET);

	//Send Command
	SetDC(RESET);
	if(HAL_SPI_Transmit_IT(&hspi4,&cmd,1)!=HAL_OK)
	{
		//fail
	}

	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	//Send Command Parameter
	SetDC(SET);
	if(HAL_SPI_Transmit_IT(&hspi4,parameter,param_size)!=HAL_OK)
	{
		//fail
	}

	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	SetCS(SET);
}

//Set Addresss Pointer
void st7735_set_address(uint8_t col_start, uint8_t col_end, uint8_t row_start, uint8_t row_end)
{
	uint8_t addr_pointer[4]={0};

/** Column Address Set
 * 80x160 해상도에서 80인 부분을 열로 사용한다면 열 주소는 26~105까지
 * 				  160인 부분을 열로 사용한다면 열 주소는 1~160까지
 */
	addr_pointer[1]=col_start;
	addr_pointer[3]=col_end;
	st7735_write_cmd_param(0x2A,addr_pointer,4);

/** Row Address Set
 * 80x160 해상도에서 80인 부분을 행으로 사용한다면 행 주소는 26~105까지
 * 				  160인 부분을 행으로 사용한다면 행 주소는 1~160까지
 */
	addr_pointer[1]=row_start;
	addr_pointer[3]=row_end;
	st7735_write_cmd_param(0x2B,addr_pointer,4);
}

//Initialize ST7735 Driver
void st7735_init(void)
{
	uint8_t cmd_param;
// Back Light
	SetLight(RESET);

// Reset Boot
	SetReset(RESET);
	HAL_Delay(10);
	SetReset(SET);
	HAL_Delay(5);

//Sleep Out
	st7735_write_cmd(0x11);
	HAL_Delay(120);

//Display On
	//st7735_write_cmd(0x29);

/** MADCTL : Memory Data Access Control
 * D7 D6 D5 D4 D3  D2
 * MY MX MV ML RGB MH
 */
 	//Exchange Column, Row + Mirror Column Address Order + Mirror RGB Order
	cmd_param=0x68;
	st7735_write_cmd_param(0x36,&cmd_param,1);

/** Addresss Set
 * Column Start, Column End, Row Start, Row End
 */
	st7735_set_address(1,160,26,105);

//Display Inversion On
	st7735_write_cmd(0x21);
}


void st7735_set_address_char(uint8_t col_start, uint8_t row_start, uint8_t font_width, uint8_t font_height)
{
	uint8_t command, addr_pointer[4]={0};

	command=0x2A;
	addr_pointer[1]=1+col_start;
	addr_pointer[3]=col_start+font_width;
	st7735_write_cmd_param(command,addr_pointer,4);

	command=0x2B;
	addr_pointer[1]=row_start+26;
	addr_pointer[3]=row_start+26+font_height+1;
	st7735_write_cmd_param(command,addr_pointer,4);
}

void st7735_write_char
(
		uint8_t chr_ascii,
		uint32_t color,
		uint8_t col,
		uint8_t row,
		const uint8_t *arrfont,
		uint8_t font_width,
		uint8_t font_height
)
{
	uint8_t *char_array;
	char_array=malloc(sizeof(uint8_t)*(3*(font_width*font_height)));
	if(!char_array) return;
	memset(char_array,0,sizeof(uint8_t)*(3*(font_width*font_height)));
	uint8_t red,green,blue;

	red=color>>16;
	green=color>>8;
	blue=color;

	for(int j=0;j<(font_width*font_height/8);j++)
	{
		for(int i=0;i<8;i++)
		{
			if(((arrfont[(chr_ascii-32)*(font_width*font_height/8)+j])>>(7-i))&0x01)
			{
				char_array[j*24+i*3]=red;
				char_array[j*24+i*3+1]=green;
				char_array[j*24+i*3+2]=blue;
			}
		}
	}

	st7735_set_address_char(col, row, font_width, font_height);

	//Memory Write
	st7735_write_cmd(0x2C);

	//Send RGB Pixel Data
	SetCS(RESET);
	SetDC(SET);

	if(HAL_SPI_Transmit_DMA(&hspi4,char_array,3*(font_width*font_height))!=HAL_OK)
	{

	}
}

void st7735_write_string
(
		char *string,
		uint32_t font_color,
		uint8_t col_start,
		uint8_t row_start,
		const uint8_t *arrfont,
		uint8_t font_width,
		uint8_t font_height
)
{
	while(*string)
	{
		st7735_write_char(*string,font_color,col_start,row_start, arrfont, font_width, font_height);

		if(col_start < 160-font_width)
		{
			col_start+=font_width;
			string++;
		}
		else
		{
			break;
		}
	}
}

void st7735_fill_screen(uint32_t color)
{
	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	uint8_t *display_buffer;

	for(int i=0; i<ST7735_WIDTH/ST7735_WIDTH_DIV; i++)
	{
		for(int j=0; j<ST7735_HEIGHT/ST7735_HEIGHT_DIV; j++)
		{
			display_buffer=(uint8_t*)malloc(sizeof(uint8_t)*(3*ST7735_WIDTH_DIV*ST7735_HEIGHT_DIV));
			if(!display_buffer) return;
			memset(display_buffer,0,sizeof(uint8_t)*(3*ST7735_WIDTH_DIV*ST7735_HEIGHT_DIV));

			for(int i=0; i<ST7735_WIDTH_DIV*ST7735_HEIGHT_DIV; i++)
			{
				display_buffer[i*3]=color>>16;
				display_buffer[i*3+1]=color>>8;
				display_buffer[i*3+2]=color;
			}

			st7735_set_address
			(
					1 + (ST7735_WIDTH_DIV * i),
					ST7735_WIDTH_DIV * (i + 1),
					26 + (ST7735_HEIGHT_DIV * j),
					ST7735_HEIGHT_DIV * (j + 1)
			);

			//Memory Write
			st7735_write_cmd(0x2C);

			//Display data transfer
			SetCS(RESET);
			SetDC(SET);

			if(HAL_SPI_Transmit_DMA(&hspi4,display_buffer,3*ST7735_WIDTH_DIV*ST7735_HEIGHT_DIV) != HAL_OK)
			{

			}
		}
	}
}

void st7735_draw_line(uint32_t color, uint8_t toppos_contour, uint8_t botpos_contour)
{
	while(HAL_SPI_GetState(&hspi4)!=HAL_SPI_STATE_READY)
	{

	}

	uint8_t *display_buffer;

	// 윗 구분선 그리기
	display_buffer=(uint8_t*)malloc(sizeof(uint8_t)*(3*ST7735_WIDTH));
	if(!display_buffer) return;
	memset(display_buffer,0,sizeof(uint8_t)*(3*ST7735_WIDTH));

	for(int i=0; i<ST7735_WIDTH; i++)
	{
		display_buffer[i*3]=color>>16;
		display_buffer[i*3+1]=color>>8;
		display_buffer[i*3+2]=color;
	}

	st7735_set_address
	(
			1,
			ST7735_WIDTH,
			26 + toppos_contour,
			26 + toppos_contour
	);

	//Memory Write
	st7735_write_cmd(0x2C);

	//Display data transfer
	SetCS(RESET);
	SetDC(SET);

	if(HAL_SPI_Transmit_DMA(&hspi4,display_buffer,3*ST7735_WIDTH) != HAL_OK)
	{

	}


	// 아랫 구분선 그리기
	display_buffer=(uint8_t*)malloc(sizeof(uint8_t)*(3*ST7735_WIDTH));
	if(!display_buffer) return;
	memset(display_buffer,0,sizeof(uint8_t)*(3*ST7735_WIDTH));

	for(int i=0; i<ST7735_WIDTH; i++)
	{
		display_buffer[i*3]=color>>16;
		display_buffer[i*3+1]=color>>8;
		display_buffer[i*3+2]=color;
	}


	st7735_set_address
	(
			1,
			ST7735_WIDTH,
			26 + botpos_contour,
			26 + botpos_contour
	);

	//Memory Write
	st7735_write_cmd(0x2C);

	//Display data transfer
	SetCS(RESET);
	SetDC(SET);

	if(HAL_SPI_Transmit_DMA(&hspi4,display_buffer,3*ST7735_WIDTH) !=HAL_OK)
	{

	}
}

void st7735_draw_string(uint32_t color)
{
	char *str1 = "Meas";
	char *str2 = "ATT";
	uint8_t display_startxpos_1st = 20;

	st7735_write_string(str1, color, display_startxpos_1st, 25, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// Meas
	st7735_write_string(str2, color, display_startxpos_1st, 63, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// ATT
}

//void st7735_example(void)
//{
//	uint8_t st_col=0, st_row=0;
//
//	st7735_fill_screen(ST7735_COLOR_BLACK);
//	HAL_Delay(1000);
//	st7735_fill_screen(ST7735_COLOR_RED);
//	HAL_Delay(1000);
//	st7735_fill_screen(ST7735_COLOR_GREEN);
//	HAL_Delay(1000);
//	st7735_fill_screen(ST7735_COLOR_BLUE);
//	HAL_Delay(1000);
//	st7735_fill_screen(ST7735_COLOR_BLACK);
//	HAL_Delay(1000);
//
//	st7735_write_string("Hello",ST7735_COLOR_RED,0,0, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	HAL_Delay(1000);
//	st7735_write_string("Hello",ST7735_COLOR_GREEN,0,FONT_HEIGHT, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	HAL_Delay(1000);
//	st7735_write_string("Hello",ST7735_COLOR_BLUE,0,FONT_HEIGHT*2, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	HAL_Delay(1000);
//
//	char font_test=' ';
//	for(int i=' ';i<='~';i++)
//	{
//	  st7735_write_char(font_test,ST7735_COLOR_RED,st_col,st_row, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	  font_test++;
//	  HAL_Delay(100);
//	  st_col+=FONT_WIDTH;
//	  if(160<=st_col)
//	  {
//		  st_col=0;
//		  if(st_row<52)
//		  {
//			  st_row+=FONT_HEIGHT;
//		  }
//		  else
//		  {
//			  st_row=0;
//		  }
//	  }
//	}
//	font_test=' ';
//	for(int i=' ';i<='~';i++)
//	{
//	  st7735_write_char(font_test,ST7735_COLOR_GREEN,st_col,st_row, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	  font_test++;
//	  HAL_Delay(100);
//	  st_col+=FONT_WIDTH;
//	  if(160<=st_col)
//	  {
//		  st_col=0;
//		  if(st_row<52)
//		  {
//			  st_row+=FONT_HEIGHT;
//		  }
//		  else
//		  {
//			  st_row=0;
//		  }
//	  }
//	}
//	font_test=' ';
//	for(int i=' ';i<='~';i++)
//	{
//	  st7735_write_char(font_test,ST7735_COLOR_BLUE,st_col,st_row, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
//	  font_test++;
//	  HAL_Delay(100);
//	  st_col+=FONT_WIDTH;
//	  if(160<=st_col)
//	  {
//		  st_col=0;
//		  if(st_row<52)
//		  {
//			  st_row+=FONT_HEIGHT;
//		  }
//		  else
//		  {
//			  st_row=0;
//		  }
//	  }
//	}
//}

void st7735_display_onoff(uint8_t status)
{
	// status : 0 >> lcd off
	// status : 1 >> lcd on

	uint8_t cmd[2] = { 0x28, 0x29 };
	uint8_t set[2] = { RESET, SET };

	st7735_write_cmd(cmd[status]);
	HAL_Delay(120);
	SetLight(set[status]);
}

void st7735_display_init()
{
	st7735_fill_screen(ST7735_COLOR_BLACK);
	st7735_draw_line(ST7735_COLOR_WHITE, 20, 60);
	st7735_draw_string(ST7735_COLOR_WHITE);
}

// Display LCD
void st7735_display_lcd
(
		uint8_t sw_dir,
		uint16_t bat_capacity,
		uint8_t in_power,
		uint32_t wavelen,
		uint8_t channel,
		int16_t opt,
		int16_t volt,
		int16_t att,
		uint8_t wifi_status
)
{
	uint8_t bat_capacity_currboundary = bat_capacity / 20;		// 0 ~ 20 ~ 40 ~ 60 ~ 80 ~
	uint8_t display_startxpos_1st = 20, display_startxpos_2nd = 75;
	uint32_t battery_color[6] =
	{
			ST7735_COLOR_RED,
			ST7735_COLOR_ORANGE,
			ST7735_COLOR_GREEN,
			ST7735_COLOR_GREEN,
			ST7735_COLOR_GREEN,
			ST7735_COLOR_BLUE
	};

	bat_capacity_currboundary = bat_capacity_currboundary > 6 ? 6 : bat_capacity_currboundary;

	// Value to String
	char str_sw_dir[2] = { 0x89, 0x88 };
	char str_bat_capacity[8] = { 0x82, 0x81, 0x80, 0x7f, 0x86, 0x85, 0x84, 0x83 };
	char str_bat_charge[2] = { ' ', 0x87 };
	char str_wavelen[20] = { 0, };
	char str_channel[10] = { 0, };
	char str_opt[20] = { 0, };
	char str_volt[20] = { 0, };
	char str_att[20] = { 0, };
	char str_sign[2] = { ' ', '-' };
	char str_wifi_status[2] = { ' ', 0x57 };

	sprintf(str_wavelen, "%c%d.%02dnm    ", str_sign[wavelen < 0], abs(wavelen / DIV_WAVE), abs(wavelen % DIV_WAVE));
	sprintf(str_opt, "%c%d.%02ddBm    ", str_sign[opt < 0], abs(opt / DIV_OPT), abs(opt % DIV_OPT));
	sprintf(str_channel, "CH%02d", channel /*- (channel <= ADC_MUX_GAP)*/);
	sprintf(str_volt, "%c%d.%03dV    ", str_sign[volt < 0], abs(volt / DIV_VOLT), abs(volt % DIV_VOLT));
	sprintf(str_att, "%c%d.%02ddB    ", str_sign[att < 0], abs(att / DIV_OPT), abs(att % DIV_OPT));

	// OptSw Dir arrow display
	st7735_write_char(str_sw_dir[sw_dir > 0], ST7735_COLOR_ORANGE, 6, 34, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);

	// batthery display
	st7735_write_char(str_bat_capacity[bat_capacity_currboundary >= 4 ? 3 : 7], battery_color[bat_capacity_currboundary], display_startxpos_1st, 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
	st7735_write_char(str_bat_capacity[bat_capacity_currboundary >= 3 ? 2 : 6], battery_color[bat_capacity_currboundary], display_startxpos_1st + 8, 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
	st7735_write_char(str_bat_capacity[bat_capacity_currboundary >= 2 ? 1 : 5], battery_color[bat_capacity_currboundary], display_startxpos_1st + 16, 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
	st7735_write_char(str_bat_capacity[bat_capacity_currboundary >= 1 ? 0 : 4], battery_color[bat_capacity_currboundary], display_startxpos_1st + 24, 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
	st7735_write_char(str_bat_charge[in_power > 0], ST7735_COLOR_BLUE, display_startxpos_1st - 10 , 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);

	// value display
	st7735_write_string(str_wavelen, ST7735_COLOR_WHITE, display_startxpos_2nd, 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// ~ nm
	st7735_write_string(str_opt, ST7735_COLOR_WHITE, display_startxpos_2nd, 25, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// ~ dBm
	st7735_write_string(str_channel, ST7735_COLOR_WHITE, display_startxpos_1st, 43, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);	// ~ CH~
	st7735_write_string(str_volt, ST7735_COLOR_WHITE, display_startxpos_2nd, 43, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// ~ V
	st7735_write_string(str_att, ST7735_COLOR_WHITE, display_startxpos_2nd, 63, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);		// ~ dB

	// wifi status display
	st7735_write_char(str_wifi_status[wifi_status > 0], ST7735_COLOR_GREEN, display_startxpos_1st + 34 , 4, d2coding_12pt, FONT_WIDTH, FONT_HEIGHT);
}




