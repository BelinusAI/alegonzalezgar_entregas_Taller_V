/*
 * SSH1106.h
 *
 *  Created on: 18/06/2023
 *      Author: alegonzalezgar
 */

#ifndef INC_SH1106_H_
#define INC_SH1106_H_

#include "I2CDriver.h"
#include "SysTickDriver.h"
#include "string.h"

//SCREEN PIXEL SIZE
#define SH1106_I2C_OLED_MAX_COLUMN					127u
#define SH1106_I2C_OLED_MAX_PAGE					7u

//I2C ADDRESS 011110(SA0)
#define SH1106_I2C_ADDRESS_1						0x3C // SA0 = 0 D7-D0 are commands
#define SH1106_I2C_ADDRESS_2						0x3D // SA0 = 1 D7-D0 are data

//CONTROL BYTES (C0)(DC)000000
#define SH1106_I2C_CONTROL_BYTE_CMD_SINGLE			0x80 // C0 = 1 D/C = 0
#define SH1106_I2C_CONTROL_BYTE_CMD_STREAM			0x00 // C0 = 0 D/C = 0
#define SH1106_I2C_CONTROL_BYTE_DATA_SINGLE			0xC0 // C0 = 1 D/C = 1
#define SH1106_I2C_CONTROL_BYTE_DATA_STREAM			0x40 // C0 = 0 D/C = 1

//COMMAND TABLE (DATASHEET PG 29)
#define SH1106_I2C_CMD_SET_COLUMN_LOWER_4			0x00
#define SH1106_I2C_CMD_SET_COLUMN_UPPER_4			0x10
#define SH1106_I2C_CMD_SET_CHARGE_PUMP_VOLTAGE		0x30 // 6.4 Vpp
#define SH1106_I2C_CMD_SET_DISPLAY_START_LINE		0x40 // Line address 0
#define SH1106_I2C_CMD_SET_CONTRAST_CONTROL_MODE	0x81
#define SH1106_I2C_CMD_SET_SEGMENT_REMAP			0xA0
#define SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_OFF		0xA5
#define SH1106_I2C_CMD_SET_ENTIRE_DISPLAY_ON		0xA4
#define SH1106_I2C_CMD_SET_DISPLAY_NORMAL			0xA6
#define SH1106_I2C_CMD_SET_DISPLAY_REVERSED			0xA7
#define SH1106_I2C_CMD_SET_MULTIPLEX_RATIO			0xA8
#define SH1106_I2C_CMD_SET_DC_DC_MODE				0xAD
#define SH1106_I2C_CMD_SET_DISPLAY_ON				0xAF
#define SH1106_I2C_CMD_SET_DISPLAY_OFF				0xAE
#define SH1106_I2C_CMD_SET_PAGE_ADDRESS				0xB0
#define SH1106_I2C_CMD_SET_COMMON_SCAN_DIRECTION	0xC0
#define SH1106_I2C_CMD_SET_DISPLAY_OFFSET_MODE		0xD3
#define SH1106_I2C_CMD_SET_OSCILLATOR_FREQUENCY		0xD5
#define SH1106_I2C_CMD_SET_DISCHARGE_PRECHARGE		0xD9
#define SH1106_I2C_CMD_COMMON_PADS_HARDWARE_CONFIG	0xDA
#define SH1106_I2C_CMD_COMMON_PADS_OUTPUT_VOLTAGE	0xDB
#define SH1106_I2C_CMD_SET_READ_MODIFY_WRITE		0xE0
#define SH1106_I2C_CMD_SET_READ_MODIFY_WRITE_END	0xEE
#define SH1106_I2C_CMD_NOP							0xE3


void OLED_Init(I2C_Handler_t *ptrHandlerI2C);
void SetDisplayOnOff(I2C_Handler_t *ptrHandlerI2C, uint8_t on);

void UpdateDisplay(I2C_Handler_t *ptrHandlerI2C);
void ClearScreen(void);

void DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void DrawLineVertical(uint8_t x, uint8_t y_start, uint8_t y_end, uint8_t color);
void DrawLineHorizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t color);
void DrawBoxEmpty(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color);
void DrawBoxFilled(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color);
void DrawCircleEmpty(int8_t x, int8_t y, int8_t radius, uint8_t color);
void DrawCircleFilled(int8_t x, int8_t y, int8_t radius, uint8_t color);
void DrawChar(char letter, uint8_t x,  uint8_t y, uint8_t color);
void DrawBitmap(uint8_t* bitmap, uint8_t x, uint8_t y, uint8_t x_len_bits, uint8_t y_len_bits, uint8_t color);

void startMenu(I2C_Handler_t *ptrHandlerI2C);

#endif /* INC_SH1106_H_ */
