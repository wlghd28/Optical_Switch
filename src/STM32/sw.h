/*
 * sw.h
 *
 *  Created on: 2023. 7. 20.
 *      Author: wise
 */

#ifndef INC_SW_H_
#define INC_SW_H_

//#define SWITCH_OPERATION_OLD

#include "hw.h"
#include <stdint.h>

#define TIME_OUT_I2C                100

#define SLAVE_ADD_EEP            	0xa0
#define INIT_VAL_EEP				0xab
#define MAX_DISPLAYBUF_INDEX_EEP	0x7f

#define AVRG_ADC_CNT				100
#define ADC_VREF					3300
#define ADC_RANGE					4096
#define ADC_DT_MAXLEN				64		// 최대 설정할 수 있는 채널 개수
#define ADC_MUX_GAP					9		// Mux 스위치 배열 1st, 2nd 간의 차이
#define ADC_MUX_LEN					8		// Mux 스위치 개수

#define EXTI_CHATTERING				200		// 200ms EXTI Chattering
#define DISPLAY_LCD_CNT				500

#define AUTO_OFF_CNT				60000	// 10ms 타이머에서 10분 동안 아무동작 안할 시 프로그램 종료
#ifdef SWITCH_OPERATION_OLD
#else
#define SW_PRETIME_PWR				300
#define SW_PRETIME_UP				5
#define SW_PRETIME_DOWN				5
#define SW_PRETIME_DIR				5
//#define SW_PRETIME_SET				5
//#define SW_PRETIME_RESET			300
#define SW_PRETIME_SAVE				5
#define SW_PRETIME_WIFI				150
#endif
#define SELMUX_CNT					5

#define DIV_VOLT					1000
#define DIV_OPT						100
#define DIV_WAVE					100

#define ALARM_BOOST_LOW				4800
#define ALARM_BOOST_HIGH			5000
#define ALARM_VBUS_LOW				4500
#define ALARM_VBUS_HIGH				5000
#define ALARM_BAT_LOW				3500
#define ALARM_BAT_HIGH				4200

enum
{
	ADC_STATE_NONE,
	ADC_STATE_SUM,
	ADC_STATE_COMPLETE
};

enum
{
	FUNC_STATE_NONE,
	FUNC_STATE_START,
	FUNC_STATE_INIT,
	FUNC_STATE_SWOPERATION,
	FUNC_STATE_PROCESSCOMBUF,
	FUNC_STATE_DETECTALARM,
	FUNC_STATE_DISPLAY
};

#pragma pack(1)
typedef union
{
	uint8_t Val;
    struct
    {
		uint8_t B0 : 1;
		uint8_t B1 : 1;
		uint8_t B2 : 1;
		uint8_t B3 : 1;
		uint8_t B4 : 1;
		uint8_t B5 : 1;
		uint8_t B6 : 1;
		uint8_t B7 : 1;
    } Bits;
} UNI_BYTE;

typedef union
{
	uint8_t Val;
    struct
    {
		uint8_t Proc : 1;
		uint8_t Func : 1;
		uint8_t Boost : 1;
		uint8_t VBus : 1;
		uint8_t Bat : 1;
		uint8_t In : 1;
		uint8_t Wifi : 1;
		uint8_t B7 : 1;
    } Bits;
} UNI_BYTE_ALARM;

typedef struct
{
	uint16_t Buf[ADC_USED_LEN];
	uint16_t Volt[ADC_USED_LEN];
	uint32_t Sum[ADC_USED_LEN];
	uint16_t nMuxBuf[MUX_ADC_LEN];
	uint8_t Cnt, Complete;
	uint8_t cSelMux;
} STC_ADC_BUF;

typedef struct
{
	uint8_t ModuleID[8];
} STC_VERSION_BUF;

typedef struct
{
	uint32_t Wavelen[ADC_DT_MAXLEN];
	int16_t Offset_A[ADC_DT_MAXLEN];
	int16_t Offset_B[ADC_DT_MAXLEN];
	int16_t Attenuation;
	uint32_t Ref_Wavelen;
	int32_t Boundary_Offset;
	int16_t Alarm_Limit[ADC_DT_MAXLEN];
	uint8_t Alarm_Enable[ADC_DT_MAXLEN];
	int16_t Alarm_Temperallimit;
}STC_DT_SAVE_BUF;

typedef struct
{
	uint16_t Vlog[ADC_DT_MAXLEN];
	int16_t OptPower[ADC_DT_MAXLEN];
}STC_DT_REALTIME_BUF;

typedef struct
{
	uint32_t Wavelen;
	int16_t OptPower;
	int16_t Attenuation;
	uint8_t OptSw;
}STC_DT_DISPLAY_BUF;

typedef struct
{
	uint16_t Boostup_Power;		// mV
	uint16_t IN_Power;			// mV
	uint16_t Bat_Power;			// mV
	uint16_t Bat_Capacity;		// m%
}STC_STATE_BUF;

typedef struct
{
	uint8_t EEP_Add_Init;	// 0xFF : non save
	uint8_t Func_State;		// FuncMain
	uint8_t SW_Pwr_Flag;	// 0 : OFF, 1 : ON
	uint16_t SW_Pwr_Pre_time; // Pressed time of Pwr Switch
	uint16_t SW_Up_Pre_time;	// Pressed time of Up Switch
	uint16_t SW_Down_Pre_time;	// Pressed time of Down Switch
	uint16_t SW_Dir_Pre_time;	// Pressed time of Dir Switch
	uint16_t SW_Set_Pre_time;	// Pressed time of Set Switch
	uint16_t SW_Save_Pre_time;	// Pressed time of Save Switch
	uint16_t SW_Wifi_Pre_time;	// Pressed time of Wifi Switch
	uint8_t SW_Set_Pre_already;	// Status of Already Pressed Set Switch
	uint8_t SW_State;		// Pressed Switch
	uint8_t CurrOptSw;		// Opt Switch (Dir) 0 : Off(Down), 1 : On(Up)
	uint8_t CurrSelChannel; // 0 ~ 17
	//uint16_t Cnt_SelMux;
	uint16_t Cnt_Display;
	uint32_t Cnt_Auto_Off;
	uint8_t Uart_recvBuf;
	uint8_t Uart_echoState;	// 0 : OFF, 1 : ON
	uint16_t nTmr;
	uint8_t EEP_stDT_Length;
	uint8_t EEP_stDT_DISPLAY_Index;
	uint8_t Wifi_State;		// 0 : OFF, 1 : ON
	UNI_BYTE_ALARM Alarm_State;
	UNI_BYTE_ALARM Alarm_Flicker;
	//uint8_t Test;
}GLOBAL_VAR;
#pragma pack()

extern GLOBAL_VAR g_Var;

extern void InitProcMain(void);
extern void InitFuncMain(void);
extern void ProcMain(void);
extern void FuncMain(void);
extern void ProcTimer(void);
extern void ReadTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);
extern void WriteTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);


#endif /* INC_SW_H_ */
