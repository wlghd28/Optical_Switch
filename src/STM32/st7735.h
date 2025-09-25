/*
 * st7735.h
 *
 *  Created on: Jul 24, 2023
 *      Author: hojin
 */

#ifndef INC_ST7735_H_
#define INC_ST7735_H_

#include <stdint.h>

#define ST7735_WIDTH 160
#define ST7735_HEIGHT 80
#define ST7735_WIDTH_DIV 20
#define ST7735_HEIGHT_DIV 10

#define ST7735_COLOR_RED 0xFC0000
#define ST7735_COLOR_ORANGE 0xFC7F00
#define ST7735_COLOR_GREEN 0x00FC00
#define ST7735_COLOR_BLUE 0x0000FC
#define ST7735_COLOR_BLACK 0x000000
#define ST7735_COLOR_WHITE 0xFFFFFF

// 함수
extern void st7735_write_cmd(uint8_t cmd);
extern void st7735_write_cmd_param(uint8_t cmd, uint8_t *parameter, uint8_t param_size);
extern void st7735_set_address(uint8_t col_start, uint8_t col_end, uint8_t row_start, uint8_t row_end);
extern void st7735_init(void);
extern void st7735_set_address_char(uint8_t col_start, uint8_t row_start, uint8_t font_width, uint8_t font_height);
extern void st7735_write_char
(
		uint8_t chr_ascii,
		uint32_t color,
		uint8_t col,
		uint8_t row,
		const uint8_t *arrfont,
		uint8_t font_width,
		uint8_t font_height
);
extern void st7735_write_string
(
		char *string,
		uint32_t font_color,
		uint8_t col_start,
		uint8_t row_start,
		const uint8_t *arrfont,
		uint8_t font_width,
		uint8_t font_height
);

extern void st7735_fill_screen(uint32_t color);
extern void st7735_draw_line(uint32_t color, uint8_t toppos_contour, uint8_t botpos_contour);
extern void st7735_draw_string(uint32_t color);
//extern void st7735_example(void);

extern void st7735_display_onoff(uint8_t status);
extern void st7735_display_init();
extern void st7735_display_lcd
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
);
#endif /* INC_ST7735_H_ */
