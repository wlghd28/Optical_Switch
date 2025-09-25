/*
 * sw.c
 *
 *  Created on: 2023. 7. 20.
 *      Author: wise
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sw.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"
#include "i2c.h"
#include "usbd_cdc_if.h"
#include "ecuation.h"
#include "st7735.h"
#include "esp8266.h"
#include "tpl1401.h"
#include "strsplit.h"
#include "ring_buffer.h"
#include "comm_protocol.h"

// sw.h
STC_ADC_BUF stADC;
STC_VERSION_BUF stVERSION;
STC_DT_SAVE_BUF stDT_SAVE[2];		// 64개 파장길이 및 Offset, Attenuation에 대한 정보 (0 : Down, 1 : Up)
STC_DT_REALTIME_BUF stDT_REALTIME;	// 18개 채널에 대한 실시간 데이터
STC_DT_DISPLAY_BUF stDT_DISPLAY;	// LCD에 표시되는 데이터
STC_STATE_BUF stSTATE;
GLOBAL_VAR g_Var;

// comm_protocol.h
STC_UI_STATUS_BUF stUI_STATUS;		// UI 프로그램으로 보내는 데이터
STC_WAVE_BUF stWAVE;
STC_OFFSET_BUF stOFFSET_A;
STC_OFFSET_BUF stOFFSET_B;
STC_ALARM_BUF stALARM;
STC_DATA_BUF stDATA;
STC_VERSIONINFO_BUF stVERSIONINFO;
STC_NETWORK_BUF stNETWORK;

void GetMuxBuf(void);
void SelMux(uint8_t cSel);
//void IncMux(void);
void GetAvrgADC(void);
void GetVlog(void);
void GetOptPower(void);
void GetStatePower(void);
//void SetAttenuation(uint8_t cSel);
void SetAttenuation(int16_t val);

void ResetAttenuation(void);
void SaveDataToEEPROM(void);
void ClearDataToEEPROM(void);
void SetDataLength(uint8_t ch);
void SetAlarmLimit(uint8_t opt_sw, int16_t val);
void SetBoundaryOffset(uint8_t opt_sw, int32_t val);
void SaveVersionInfo(uint8_t* module_id);
void LoadVersionInfo(void);
void SaveNetworkInfo(uint8_t* wifi_id, uint8_t* wifi_pwd, uint8_t ip);
void LoadNetworkInfo(void);
void SwitchOperation(void);
void DetectAlarm(void);
void DisplayAlarm(void);
void ProcTimer(void);

// I2C
uint8_t CheckBusyI2C(I2C_HandleTypeDef* hi2c, uint16_t DevAddress);
void ReadsEEP(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);
void WritesEEP(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);
void ReadTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);
void WriteTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen);

// Communication
uint16_t Search_StrCmd(const char** arr, char* target, uint16_t arrlen);
void ReceiveCombuf_DEBUG(uint8_t* Buf, uint32_t Len);
void ProcessCombuf_DEBUG(void);
void ReceiveCombuf_USB(uint8_t* Buf, uint32_t Len);
void ProcessCombuf_USB(void);
void ProcessComCmd_USB(uint16_t cmd, const char** strarr);
void SendUIStatus_USB(void);
void ReceiveCombuf_WIFI(uint8_t* Buf, uint32_t Len);
void ProcessCombuf_WIFI(void);
void ProcessComCmd_WIFI(uint16_t cmd, uint8_t id, const char** strarr);
void ProcessComSend_WIFI(void);
void SendUIStatus_WIFI(void);


void InitProcMain(void)
{
	memset(&stADC, 0, sizeof(stADC));
	HAL_TIM_Base_Start_IT(&htim4);	//10mS
	//HAL_ADCEx_Calibration_Start(&hadc1); // STM324XX 시리즈에선 지원 x
	HAL_ADC_Start_DMA(&hadc1, (uint16_t*) stADC.Buf, ADC_USED_LEN);
}

void InitFuncMain(void)
{
	//ClearDataToEEPROM();

	// memory set
	memset(&g_Var, 0, sizeof(GLOBAL_VAR));
	g_Var.Func_State = FUNC_STATE_START;
	//g_Var.SW_State = SW_NONE;
	g_Var.SW_State = SW_PWR;
	g_Var.Uart_echoState = 1;
	g_Var.CurrOptSw = GetOptSW();
	//SetOptSW(g_Var.CurrOptSw);
	//SelMux(0);

	// st7735 lcd initialize
	st7735_init();
	st7735_display_init();
	//st7735_example();

	// esp8226 wifi module start
	//esp8266_start(0);

	// tpl1401 initialize
	tpl1401_init();
	//tpl1401_Set_Dpot_Pos(0XFF);

	// Uart Interrupt
	if(HAL_UART_Receive_IT(&huart1, &(g_Var.Uart_recvBuf), 1) != HAL_OK)
	{
		//printf("Fail UART1 Receive IT\r\n");
	}
	if(HAL_UART_Receive_IT(&huart2, &(g_Var.Uart_recvBuf), 1) != HAL_OK)
	{
		//printf("Fail UART3 Receive IT\r\n");
	}

	// Read SaveData From EEPROM
	// 이전에 EEPROM에 데이터가 저장되어 있는지 확인
	ReadsEEP(EEP_ADD_INIT, &g_Var.EEP_Add_Init, 1);
	if (g_Var.EEP_Add_Init == INIT_VAL_EEP)
	{
		ReadsEEP(EEP_ADD_DT, (uint8_t *)stDT_SAVE, sizeof(stDT_SAVE));
		ReadsEEP(EEP_ADD_VERSION, (uint8_t*)&stVERSION, sizeof(stVERSION));
		ReadsEEP(EEP_ADD_NETWORKINFO, (uint8_t*)&esp8266_NETWORK, sizeof(esp8266_NETWORK));
	}
	else
	{
		memset(stDT_SAVE, 0, sizeof(stDT_SAVE));

		uint8_t eep_add_init = INIT_VAL_EEP;
		uint8_t cBuf = 0;
		size_t cSize = 0;

		cSize += sizeof(uint8_t);	// stDT Length
		cSize += sizeof(stDT_SAVE); // stDT_SAVE
		cSize += sizeof(uint8_t);	// stDT_DISPLAY Index;

		WritesEEP(EEP_ADD_INIT, &eep_add_init, sizeof(uint8_t));
		for(int i = 0; i < cSize; i++)
		{
			WritesEEP(EEP_ADD_DTLEN + i, &cBuf, 1);
		}

		memset(&stVERSION, 0, sizeof(stVERSION));
		WritesEEP(EEP_ADD_VERSION, (uint8_t*)&stVERSION, sizeof(stVERSION));

		memset(&esp8266_NETWORK, 0, sizeof(esp8266_NETWORK));
		esp8266_NETWORK.IP[3] = 111;
		WritesEEP(EEP_ADD_NETWORKINFO, (uint8_t*)&esp8266_NETWORK, sizeof(esp8266_NETWORK));
	}

	ReadsEEP(EEP_ADD_DTLEN, &g_Var.EEP_stDT_Length, sizeof(uint8_t));
	if(g_Var.EEP_stDT_Length > ADC_DT_MAXLEN || g_Var.EEP_stDT_Length == 0)	g_Var.EEP_stDT_Length = ADC_DT_MAXLEN;

	ReadsEEP(EEP_ADD_DT + sizeof(stDT_SAVE), &g_Var.EEP_stDT_DISPLAY_Index, sizeof(uint8_t));
	if(g_Var.EEP_stDT_DISPLAY_Index > MAX_DISPLAYBUF_INDEX_EEP)	g_Var.EEP_stDT_DISPLAY_Index = 0;

}

void ProcMain(void)
{
	switch (stADC.Complete)
	{
		case ADC_STATE_SUM :
			stADC.Complete = ADC_STATE_NONE;
			GetAvrgADC();
			break;

		case ADC_STATE_COMPLETE :
			stADC.Complete = ADC_STATE_NONE;
			// Call calc function
			GetVlog();
			GetOptPower();
			GetMuxBuf();
			// if(++g_Var.Cnt_SelMux >= SELMUX_CNT)
			// {
			// 	GetMuxBuf();
			// 	g_Var.Cnt_SelMux = 0;
			// }
			GetStatePower();
			break;

		default:
			break;
	}
}

void FuncMain(void)
{
	switch (g_Var.Func_State)
	{
		case FUNC_STATE_START :
//			if(GetPwrSW())
//			{
//				if(++g_Var.SW_PwrON_Pre_time > SW_PRETIME_PWRON)
//				{
//					SetOnOff(1);
//					st7735_display_onoff(1);
//					g_Var.SW_PwrON_Pre_time = 0;
//					g_Var.Func_State = FUNC_STATE_INIT;
//				}
//			}
			g_Var.Func_State = FUNC_STATE_SWOPERATION;
			break;

		case FUNC_STATE_INIT:
			g_Var.Func_State = FUNC_STATE_SWOPERATION;
			esp8266_init();
			esp8266_startserver_TCP();
			break;

		case FUNC_STATE_SWOPERATION:
			g_Var.Func_State = FUNC_STATE_PROCESSCOMBUF;
			SwitchOperation();
			if(g_Var.Cnt_Auto_Off > AUTO_OFF_CNT)
			{
				g_Var.SW_Pwr_Flag = 0;
				g_Var.Func_State = FUNC_STATE_START;
				st7735_display_onoff(0);
				SetOnOff(0);
			}
			break;

		case FUNC_STATE_PROCESSCOMBUF:
			g_Var.Func_State = FUNC_STATE_DETECTALARM;
			ProcessCombuf_DEBUG();
			ProcessCombuf_USB();
			ProcessCombuf_WIFI();
			break;

		case FUNC_STATE_DETECTALARM :
			g_Var.Func_State = FUNC_STATE_DISPLAY;
			DetectAlarm();
			DisplayAlarm();
			break;

		case FUNC_STATE_DISPLAY :
			g_Var.Func_State = FUNC_STATE_SWOPERATION;
			if(++g_Var.Cnt_Display >= DISPLAY_LCD_CNT)
			{
				// Display LCD
				st7735_display_lcd
				(
						g_Var.CurrOptSw,
						stSTATE.Bat_Capacity,
						g_Var.Alarm_State.Bits.In,
						stDT_SAVE[g_Var.CurrOptSw].Wavelen[g_Var.CurrSelChannel],
						g_Var.CurrSelChannel + 1,
						stDT_REALTIME.OptPower[g_Var.CurrSelChannel],
						stDT_REALTIME.Vlog[g_Var.CurrSelChannel],
						stDT_SAVE[g_Var.CurrOptSw].Attenuation,
						g_Var.Wifi_State
				);
				g_Var.Cnt_Display = 0;
			}
			break;

		default:
			break;
	}
}

void GetMuxBuf(void)
{
	stADC.nMuxBuf[stADC.cSelMux] = stADC.Volt[ADC_1ST];
	stADC.nMuxBuf[stADC.cSelMux+8] = stADC.Volt[ADC_2ND];
	stADC.nMuxBuf[stADC.cSelMux+16] = stADC.Volt[ADC_3TH];
	//if(++stADC.cSelMux >= ADC_MUX_LEN) stADC.cSelMux = 0;
	stADC.cSelMux = (stADC.cSelMux + 1) & (ADC_MUX_LEN - 1);
	SelMux(stADC.cSelMux);
}

void SelMux(uint8_t cSel)
{
	UNI_BYTE cMux;
	cMux.Val = cSel;
	SetMux0(cMux.Bits.B0);
	SetMux1(cMux.Bits.B1);
	SetMux2(cMux.Bits.B2);
}

//void IncMux()
//{
//	g_Var.CurrSelMux = (g_Var.CurrSelMux + 1) & (ADC_MUX_LEN - 1);
//	SelMux(g_Var.CurrSelMux);
//}

void GetAvrgADC(void)
{
	uint8_t i;
	for (i=0; i<ADC_USED_LEN; i++)
		stADC.Sum[i] += stADC.Buf[i];
	if (++stADC.Cnt >= AVRG_ADC_CNT)
	{
		for (i=0; i<ADC_USED_LEN; i++)
		{

			stADC.Volt[i] = (uint16_t) ((stADC.Sum[i] / AVRG_ADC_CNT) * ADC_VREF / ADC_RANGE);
			stADC.Sum[i] = 0;
		}
		stADC.Cnt = 0;
		stADC.Complete = ADC_STATE_COMPLETE;
	}
}

void GetVlog(void)
{
	stDT_REALTIME.Vlog[8] = stADC.nMuxBuf[MUX_PD_8];
	stDT_REALTIME.Vlog[17] = stADC.nMuxBuf[MUX_PD_18];

	stDT_REALTIME.Vlog[stADC.cSelMux] = stADC.nMuxBuf[stADC.cSelMux];
	stDT_REALTIME.Vlog[stADC.cSelMux + ADC_MUX_GAP] = stADC.nMuxBuf[stADC.cSelMux + ADC_MUX_LEN];
}

void GetOptPower(void)
{
	stDT_REALTIME.OptPower[stADC.cSelMux] =
				Optical_Power_Detection
				(
						stADC.nMuxBuf[stADC.cSelMux],
						DIV_VOLT,
						DIV_OPT
				);

	stDT_REALTIME.OptPower[8] =
				Optical_Power_Detection
				(
						stADC.nMuxBuf[MUX_PD_8],
						DIV_VOLT,
						DIV_OPT
				);

	stDT_REALTIME.OptPower[stADC.cSelMux + ADC_MUX_GAP] =
				Optical_Power_Detection
				(
						stADC.nMuxBuf[stADC.cSelMux + ADC_MUX_LEN],
						DIV_VOLT,
						DIV_OPT
				);

	stDT_REALTIME.OptPower[17] =
				Optical_Power_Detection
				(
						stADC.nMuxBuf[MUX_PD_18],
						DIV_VOLT,
						DIV_OPT
				);

	stDT_REALTIME.OptPower[stADC.cSelMux] =
				Optical_Power_ApplyOffset
				(
						stDT_REALTIME.OptPower[stADC.cSelMux],
						stDT_SAVE[g_Var.CurrOptSw].Offset_A[stADC.cSelMux] * (stDT_REALTIME.OptPower[stADC.cSelMux] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						stDT_SAVE[g_Var.CurrOptSw].Offset_B[stADC.cSelMux] * (stDT_REALTIME.OptPower[stADC.cSelMux] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						DIV_OPT
				);

	stDT_REALTIME.OptPower[8] =
				Optical_Power_ApplyOffset
				(
						stDT_REALTIME.OptPower[8],
						stDT_SAVE[g_Var.CurrOptSw].Offset_A[8] * (stDT_REALTIME.OptPower[8] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						stDT_SAVE[g_Var.CurrOptSw].Offset_B[8] * (stDT_REALTIME.OptPower[8] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						DIV_OPT
				);

	stDT_REALTIME.OptPower[stADC.cSelMux + ADC_MUX_GAP] =
				Optical_Power_ApplyOffset
				(
						stDT_REALTIME.OptPower[stADC.cSelMux + ADC_MUX_GAP],
						stDT_SAVE[g_Var.CurrOptSw].Offset_A[stADC.cSelMux + ADC_MUX_GAP] * (stDT_REALTIME.OptPower[stADC.cSelMux + ADC_MUX_GAP] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						stDT_SAVE[g_Var.CurrOptSw].Offset_B[stADC.cSelMux + ADC_MUX_GAP] * (stDT_REALTIME.OptPower[stADC.cSelMux + ADC_MUX_GAP] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						DIV_OPT
				);

	stDT_REALTIME.OptPower[17] =
				Optical_Power_ApplyOffset
				(
						stDT_REALTIME.OptPower[17],
						stDT_SAVE[g_Var.CurrOptSw].Offset_A[17] * (stDT_REALTIME.OptPower[17] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						stDT_SAVE[g_Var.CurrOptSw].Offset_B[17] * (stDT_REALTIME.OptPower[17] >= stDT_SAVE[g_Var.CurrOptSw].Boundary_Offset),
						DIV_OPT
				);
}

void GetStatePower(void)
{
	double
		boostup_power = 0,
		in_power = 0,
		battery_power = 0,
		battery_capacity = 0;

	uint16_t
		//r1_boostup = 10000,
		r2_boostup = 15000,
		rs_boostup = 25000,
		//r1_inp = 10000,
		r2_in = 15000,
		rs_in = 25000,
		//r1_bat = 4700,
		r2_bat = 15000,
		rs_bat = 19700;

	// stADC.Volt 값 >> Vo
	// 계산하여 얻을 값 >> Vi
	// Vi = Vo * (R1 + R2) / R2
	// Boostup Power
	boostup_power = (double)stADC.nMuxBuf[MUX_BOOST] * (double)rs_boostup;
	boostup_power /= (double)r2_boostup;
	stSTATE.Boostup_Power = (uint16_t)boostup_power;

	// IN Power
	in_power = (double)stADC.nMuxBuf[MUX_INP] * (double)rs_in;
	in_power /= (double)r2_in;
	stSTATE.IN_Power = (uint16_t)in_power;

	// Battery Power (High : 4.2V, Low : 3.5V)
	battery_power = (double)stADC.nMuxBuf[MUX_BAT] * (double)rs_bat;
	battery_power /= (double)r2_bat;
	stSTATE.Bat_Power = (uint16_t)battery_power;

	// Battery Capacity	(High : 100%, Low : 0%)
	// y = 100 * (x - ALARM_BAT_LOW) / (ALARM_BAT_HIGH - ALARM_BAT_LOW) (x : battery_power, y : battery_capacity)
	battery_capacity = battery_power;
	battery_capacity -= (double)ALARM_BAT_LOW;
	battery_capacity *= (double)100;
	battery_capacity /= (double)(ALARM_BAT_HIGH - ALARM_BAT_LOW);
	stSTATE.Bat_Capacity = (uint16_t)battery_capacity;
}

//void SetAttenuation(uint8_t cSel)
//{
//	stDT_SAVE[0].Attenuation = stDT_REALTIME.OptPower[cSel];
//	stDT_SAVE[0].Ref_Wavelen = stDT_SAVE[g_Var.CurrOptSw].Wavelen[cSel];
//
//	// Write Attenuation, Ref_wavelen To EEPROM
//	WritesEEP(
//		EEP_ADD_DT +
//		sizeof(stDT_SAVE[0].Wavelen) +
//		sizeof(stDT_SAVE[0].Offset_A) +
//		sizeof(stDT_SAVE[0].Offset_B),
//		(uint8_t *)&(stDT_SAVE[0].Attenuation), sizeof(int16_t));
//
//	WritesEEP(
//		EEP_ADD_DT +
//		sizeof(stDT_SAVE[0].Wavelen) +
//		sizeof(stDT_SAVE[0].Offset_A) +
//		sizeof(stDT_SAVE[0].Offset_B) +
//		sizeof(int16_t),
//		(uint8_t *)&(stDT_SAVE[0].Ref_Wavelen), sizeof(uint32_t));
//}

void SetAttenuation(int16_t val)
{
	stDT_SAVE[0].Attenuation = val;
	stDT_SAVE[1].Attenuation = val;

	// Write Attenuation, Ref_wavelen To EEPROM
	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0].Wavelen) +
		sizeof(stDT_SAVE[0].Offset_A) +
		sizeof(stDT_SAVE[0].Offset_B),
		(uint8_t *)&(stDT_SAVE[0].Attenuation), sizeof(int16_t));

	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0].Wavelen) +
		sizeof(stDT_SAVE[0].Offset_A) +
		sizeof(stDT_SAVE[0].Offset_B) +
		sizeof(int16_t),
		(uint8_t *)&(stDT_SAVE[0].Ref_Wavelen), sizeof(uint32_t));

	WritesEEP(
			EEP_ADD_DT +
			sizeof(stDT_SAVE[0]) +
			sizeof(stDT_SAVE[1].Wavelen) +
			sizeof(stDT_SAVE[1].Offset_A) +
			sizeof(stDT_SAVE[1].Offset_B),
			(uint8_t *)&(stDT_SAVE[1].Attenuation), sizeof(int16_t));

	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0]) +
		sizeof(stDT_SAVE[1].Wavelen) +
		sizeof(stDT_SAVE[1].Offset_A) +
		sizeof(stDT_SAVE[1].Offset_B) +
		sizeof(int16_t),
		(uint8_t *)&(stDT_SAVE[1].Ref_Wavelen), sizeof(uint32_t));

	// Write TPL1401
	//g_Var.Test = Dpot_Decimal_Value(val, DIV_OPT);
	tpl1401_Set_Dpot_Pos(Dpot_Decimal_Value(val, DIV_OPT));
}

void ResetAttenuation()
{
	stDT_SAVE[0].Attenuation = 0;
	stDT_SAVE[0].Ref_Wavelen = 0;
	stDT_SAVE[1].Attenuation = 0;
	stDT_SAVE[1].Ref_Wavelen = 0;

	// Write Attenuation, Ref_wavelen To EEPROM
	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0].Wavelen) +
		sizeof(stDT_SAVE[0].Offset_A) +
		sizeof(stDT_SAVE[0].Offset_B),
		(uint8_t *)&(stDT_SAVE[0].Attenuation), sizeof(int16_t));

	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0].Wavelen) +
		sizeof(stDT_SAVE[0].Offset_A) +
		sizeof(stDT_SAVE[0].Offset_B) +
		sizeof(int16_t),
		(uint8_t *)&(stDT_SAVE[0].Ref_Wavelen), sizeof(uint32_t));

	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0]) +
		sizeof(stDT_SAVE[1].Wavelen) +
		sizeof(stDT_SAVE[1].Offset_A) +
		sizeof(stDT_SAVE[1].Offset_B),
		(uint8_t *)&(stDT_SAVE[1].Attenuation), sizeof(int16_t));

	WritesEEP(
		EEP_ADD_DT +
		sizeof(stDT_SAVE[0]) +
		sizeof(stDT_SAVE[1].Wavelen) +
		sizeof(stDT_SAVE[1].Offset_A) +
		sizeof(stDT_SAVE[1].Offset_B) +
		sizeof(int16_t),
		(uint8_t *)&(stDT_SAVE[1].Ref_Wavelen), sizeof(uint32_t));

	// Write TPL1401
	tpl1401_Set_Dpot_Pos(0);
}

void SaveDataToEEPROM()
{
	// Write Display Data To EEPROM
	stDT_DISPLAY.Wavelen = stDT_SAVE[g_Var.CurrOptSw].Wavelen[g_Var.CurrSelChannel];
	stDT_DISPLAY.OptPower = stDT_REALTIME.OptPower[g_Var.CurrSelChannel];
	stDT_DISPLAY.Attenuation = stDT_REALTIME.OptPower[g_Var.CurrSelChannel] - stDT_SAVE[g_Var.CurrOptSw].Attenuation;
	stDT_DISPLAY.OptSw = g_Var.CurrOptSw;

	WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE) + sizeof(uint8_t) + (sizeof(stDT_DISPLAY) * g_Var.EEP_stDT_DISPLAY_Index),
			(uint8_t *)&stDT_DISPLAY, sizeof(stDT_DISPLAY));

	// Add Index
	g_Var.EEP_stDT_DISPLAY_Index = ++g_Var.EEP_stDT_DISPLAY_Index > MAX_DISPLAYBUF_INDEX_EEP ? 0 : g_Var.EEP_stDT_DISPLAY_Index;

	// Write Display Data Index To EEPROM
	WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE), &g_Var.EEP_stDT_DISPLAY_Index, sizeof(uint8_t));
}

void ClearDataToEEPROM()
{
	uint8_t cBuf = 0xFF;
	size_t cSize = 0;
	cSize += sizeof(uint8_t);	// stDT Length
	cSize += sizeof(stDT_SAVE); // stDT_SAVE[g_Var.CurrOptSw]
	cSize += sizeof(uint8_t);	// stDT_DISPLAY Index;
	cSize += sizeof(STC_DT_DISPLAY_BUF) * (MAX_DISPLAYBUF_INDEX_EEP + 1);	// 128 * stDT_DISPLAY

	WritesEEP(EEP_ADD_INIT, &cBuf, 1);
	for(int i = 0; i < cSize; i++)
	{
		WritesEEP(EEP_ADD_DTLEN + i, &cBuf, 1);
	}

	g_Var.EEP_stDT_DISPLAY_Index = 0;
}

void SetDataLength(uint8_t len)
{
	WritesEEP(EEP_ADD_DTLEN, &len, sizeof(uint8_t));
	g_Var.EEP_stDT_Length = len;
	g_Var.CurrSelChannel = 0;
//	g_Var.CurrSelMux = 0;
//	SelMux(0);
}

void SetAlarmLimit(uint8_t opt_sw, int16_t val)
{
	stDT_SAVE[opt_sw].Alarm_Temperallimit = val;
	WritesEEP(
		EEP_ADD_DT +
		(sizeof(STC_DT_SAVE_BUF) * opt_sw) +
		sizeof(stDT_SAVE[opt_sw].Wavelen) +
		sizeof(stDT_SAVE[opt_sw].Offset_A) +
		sizeof(stDT_SAVE[opt_sw].Offset_B) +
		sizeof(stDT_SAVE[opt_sw].Attenuation) +
		sizeof(stDT_SAVE[opt_sw].Ref_Wavelen) +
		sizeof(stDT_SAVE[opt_sw].Boundary_Offset) +
		sizeof(stDT_SAVE[opt_sw].Alarm_Limit) +
		sizeof(stDT_SAVE[opt_sw].Alarm_Enable),
		(uint8_t *)&stDT_SAVE[opt_sw].Alarm_Temperallimit,
		sizeof(stDT_SAVE[opt_sw].Alarm_Temperallimit));
}

void SetBoundaryOffset(uint8_t opt_sw, int32_t val)
{
	stDT_SAVE[opt_sw].Boundary_Offset = val;
	WritesEEP(
		EEP_ADD_DT +
		(sizeof(STC_DT_SAVE_BUF) * opt_sw) +
		sizeof(stDT_SAVE[opt_sw].Wavelen) +
		sizeof(stDT_SAVE[opt_sw].Offset_A) +
		sizeof(stDT_SAVE[opt_sw].Offset_B) +
		sizeof(stDT_SAVE[opt_sw].Attenuation) +
		sizeof(stDT_SAVE[opt_sw].Ref_Wavelen),
		(uint8_t *)&stDT_SAVE[opt_sw].Boundary_Offset,
		sizeof(stDT_SAVE[opt_sw].Boundary_Offset));
}

void SaveVersionInfo(uint8_t* module_id)
{
	memset(stVERSION.ModuleID, 0, sizeof(stVERSION.ModuleID));

	// Save to EEPROM
	memcpy(stVERSION.ModuleID,module_id, strlen((char*)module_id));

	WritesEEP(EEP_ADD_VERSION, (uint8_t *)&stVERSION, sizeof(stVERSION));
}

void LoadVersionInfo(void)
{
	memset(&stVERSIONINFO, 0, sizeof(stVERSIONINFO));

	stVERSIONINFO.Cmd = COMM_CMD_LOAD_VERSION;
	ReadsEEP(EEP_ADD_VERSION, (uint8_t*)stVERSIONINFO.ModuleID, sizeof(stVERSIONINFO));
}

void SaveNetworkInfo(uint8_t* wifi_id, uint8_t* wifi_pwd, uint8_t ip)
{
	memset(&esp8266_NETWORK, 0, sizeof(esp8266_NETWORK));

	// Save to EEPROM
	memcpy(esp8266_NETWORK.ID, wifi_id, strlen((char*)wifi_id));
	memcpy(esp8266_NETWORK.Pwd, wifi_pwd, strlen((char*)wifi_pwd));
	esp8266_NETWORK.IP[3] = ip;

	WritesEEP(EEP_ADD_NETWORKINFO, (uint8_t *)&esp8266_NETWORK, sizeof(esp8266_NETWORK));

	esp8266_set_networkinfo(wifi_id, wifi_pwd, ip);
}

void LoadNetworkInfo()
{
	// Laod from EEPROM
	ReadsEEP(EEP_ADD_NETWORKINFO, (uint8_t*)&stNETWORK, sizeof(stNETWORK));
	stNETWORK.Cmd = COMM_CMD_LOAD_NETWORKINFO;
}

void SwitchOperation(void)
{

	switch(g_Var.SW_State)
	{
	case SW_NONE:
		break;

	case SW_PWR:
#ifdef SWITCH_OPERATION_OLD

#else
//		if(GetPwrSW())
//		{
//			if(g_Var.SW_Pwr_Flag)
//			{
//				if(g_Var.SW_Pwr_Pre_time > SW_PRETIME_PWR)
//				{
//					g_Var.SW_Pwr_Flag = 0;
//					g_Var.SW_Pwr_Pre_time = 0;
//					g_Var.SW_State = SW_NONE;
//					g_Var.Func_State = FUNC_STATE_START;
//					st7735_display_onoff(0);
//					SetOnOff(0);
//				}
//
//				if(g_Var.SW_Wifi_Pre_time > SW_PRETIME_WIFI)
//				{
//					g_Var.Wifi_State = g_Var.Wifi_State ? 0 : 1;
//					g_Var.SW_Wifi_Pre_time = 0;
//				}
//			}
//			else
//			{
//				if(g_Var.SW_Pwr_Pre_time > SW_PRETIME_PWR)
//				{
//					SetOnOff(1);
//					st7735_display_onoff(1);
//					g_Var.SW_Pwr_Pre_time = 0;
//					g_Var.SW_State = SW_NONE;
//					g_Var.Func_State = FUNC_STATE_SWOPERATION;
//					g_Var.SW_Pwr_Flag = 1;
//				}
//			}
//
//		}
//		else
//		{
//			g_Var.SW_Wifi_Pre_time = 0;
//			g_Var.SW_Pwr_Pre_time = 0;
//			g_Var.SW_State = SW_WIFI;
//		}

		if(GetPwrSW())
		{
			if(g_Var.SW_Pwr_Pre_time > SW_PRETIME_PWR)
			{
				if(g_Var.SW_Pwr_Flag)
				{
					g_Var.Alarm_State.Bits.Proc ^= 1;
					g_Var.SW_Pwr_Flag = 0;
					g_Var.SW_Pwr_Pre_time = 0;
					g_Var.SW_State = SW_NONE;
					g_Var.Func_State = FUNC_STATE_START;
					st7735_display_onoff(0);
					SetOnOff(0);
				}
				else
				{
					SetOnOff(1);
					st7735_display_onoff(1);
					g_Var.SW_Pwr_Pre_time = 0;
					g_Var.SW_State = SW_NONE;
					g_Var.Func_State = FUNC_STATE_SWOPERATION;
					g_Var.SW_Pwr_Flag = 1;
					g_Var.Alarm_State.Bits.Proc ^= 1;
				}
			}
		}
		else
		{
			g_Var.SW_Pwr_Pre_time = 0;
			g_Var.SW_State = SW_NONE;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_UP:
#ifdef SWITCH_OPERATION_OLD

#else
		if(GetUpSW())
		{
			if(g_Var.SW_Up_Pre_time > SW_PRETIME_UP)
			{
				g_Var.CurrSelChannel = ++g_Var.CurrSelChannel < g_Var.EEP_stDT_Length ? g_Var.CurrSelChannel : 0;
				g_Var.SW_Up_Pre_time = 0;
				g_Var.SW_State = SW_NONE;
			}
		}
		else
		{
			g_Var.SW_Up_Pre_time = 0;
			g_Var.SW_State = SW_NONE;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_DOWN:
#ifdef SWITCH_OPERATION_OLD

#else
		if(GetDownSW())
		{
			if(g_Var.SW_Down_Pre_time > SW_PRETIME_DOWN)
			{
				g_Var.CurrSelChannel = --g_Var.CurrSelChannel == 0xFF ? g_Var.EEP_stDT_Length - 1 : g_Var.CurrSelChannel;
				g_Var.SW_Down_Pre_time = 0;
				g_Var.SW_State = SW_NONE;
			}
		}
		else
		{
			g_Var.SW_Down_Pre_time = 0;
			g_Var.SW_State = SW_NONE;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_DIR:
#ifdef SWITCH_OPERATION_OLD

#else
		if(GetDirSW())
		{
			if(g_Var.SW_Dir_Pre_time > SW_PRETIME_DIR)
			{
				g_Var.CurrOptSw = !g_Var.CurrOptSw;
				SetOptSW(g_Var.CurrOptSw);
				g_Var.SW_Dir_Pre_time = 0;
				g_Var.SW_State = SW_NONE;
			}
		}
		else
		{
			g_Var.SW_Dir_Pre_time = 0;
			g_Var.SW_State = SW_NONE;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_SET:
#ifdef SWITCH_OPERATION_OLD

#else
//		if(GetSetSW())
//		{
//			if(g_Var.SW_Set_Pre_time > SW_PRETIME_RESET)
//			{
//				ResetAttenuation();
//				g_Var.SW_Set_Pre_already = 0;
//				g_Var.SW_Set_Pre_time = 0;
//				g_Var.SW_State = SW_NONE;
//			}
//			else if(g_Var.SW_Set_Pre_time > SW_PRETIME_SET && !g_Var.SW_Set_Pre_already)
//			{
//				SetAttenuation(g_Var.CurrSelChannel);
//				g_Var.SW_Set_Pre_already = 1;
//			}
//		}
//		else
//		{
//			g_Var.SW_Set_Pre_already = 0;
//			g_Var.SW_Set_Pre_time = 0;
//			g_Var.SW_State = SW_NONE;
//		}

		if(GetSetSW())
		{
			if(g_Var.SW_Wifi_Pre_time > SW_PRETIME_WIFI)
			{
				g_Var.Alarm_State.Bits.Wifi ^= 1;
				g_Var.Wifi_State = !g_Var.Wifi_State;
				g_Var.SW_Wifi_Pre_time = 0;
				g_Var.SW_State = SW_WIFI;
			}
		}
		else
		{
			g_Var.SW_Wifi_Pre_time = 0;
			g_Var.SW_State = SW_WIFI;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_SAVE:
#ifdef SWITCH_OPERATION_OLD

#else
		if(GetSaveSW())
		{
			if(g_Var.SW_Save_Pre_time > SW_PRETIME_SAVE)
			{
				SaveDataToEEPROM();
				g_Var.SW_Save_Pre_time = 0;
				g_Var.SW_State = SW_NONE;
			}
		}
		else
		{
			g_Var.SW_Save_Pre_time = 0;
			g_Var.SW_State = SW_NONE;
		}
#endif
		g_Var.Cnt_Auto_Off = 0;
		break;

	case SW_WIFI:
		if(g_Var.Wifi_State)
		{
			esp8266_start(1);
			g_Var.Func_State = FUNC_STATE_INIT;
		}
		else
		{
			esp8266_start(0);
		}
		g_Var.SW_State = SW_NONE;
		break;

	default:
		break;
	}
}


void DetectAlarm(void)
{
	// Boost up
	g_Var.Alarm_State.Bits.Boost = (stSTATE.Boostup_Power < ALARM_BOOST_LOW || stSTATE.Boostup_Power > ALARM_BOOST_HIGH);

	// VBus
	g_Var.Alarm_State.Bits.VBus = (stSTATE.IN_Power < ALARM_VBUS_LOW || stSTATE.IN_Power > ALARM_VBUS_HIGH);

	// Battery
	g_Var.Alarm_State.Bits.Bat = (stSTATE.Bat_Power <= ALARM_BAT_LOW || stSTATE.Bat_Power > ALARM_BAT_HIGH);

	// IN Power
	g_Var.Alarm_State.Bits.In = (stSTATE.IN_Power > ALARM_VBUS_LOW);
}


void DisplayAlarm(void)
{
	// Alarm_Flicker 값에 따라 LED 조작
	// MCU Status
	SetFstG(!(g_Var.Alarm_State.Bits.Proc & g_Var.Alarm_Flicker.Bits.Proc));

	// WIfi Status
	SetSndG(!(g_Var.Alarm_State.Bits.Wifi & g_Var.Alarm_Flicker.Bits.Wifi));

	// Boost up
	//SetFstG(!(g_Var.Alarm_State.Bits.Boost & g_Var.Alarm_Flicker.Bits.Boost));

	// VBus
	SetFstR(!(g_Var.Alarm_State.Bits.VBus & g_Var.Alarm_Flicker.Bits.VBus));

	// Battery
	SetSndR(!(g_Var.Alarm_State.Bits.Bat & g_Var.Alarm_Flicker.Bits.Bat));
}


// Callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (stADC.Complete != ADC_STATE_COMPLETE) stADC.Complete = ADC_STATE_SUM;
}

// 10ms timer
void ProcTimer(void)
{
	// Update SW Pressed Status
	g_Var.SW_Pwr_Pre_time = g_Var.SW_Pwr_Pre_time * (GetPwrSW() & (g_Var.SW_State == SW_PWR)) + 1;
	g_Var.SW_Up_Pre_time = g_Var.SW_Up_Pre_time * (GetUpSW() & (g_Var.SW_State == SW_UP)) + 1;
	g_Var.SW_Down_Pre_time = g_Var.SW_Down_Pre_time * (GetDownSW() & (g_Var.SW_State == SW_DOWN)) + 1;
	g_Var.SW_Dir_Pre_time = g_Var.SW_Dir_Pre_time * (GetDirSW() & (g_Var.SW_State == SW_DIR)) + 1;
	//g_Var.SW_Set_Pre_time = g_Var.SW_Set_Pre_time * (GetSetSW() & (g_Var.SW_State == SW_SET)) + 1;
	g_Var.SW_Save_Pre_time = g_Var.SW_Save_Pre_time * (GetSaveSW() & (g_Var.SW_State == SW_SAVE)) + 1;
	g_Var.SW_Wifi_Pre_time = g_Var.SW_Wifi_Pre_time * (GetSetSW() & (g_Var.SW_State == SW_SET)) + 1;

	// Auto Off
	g_Var.Cnt_Auto_Off++;

	// LED
	if((g_Var.nTmr % 50) == 0)
	{
		g_Var.Alarm_Flicker.Bits.Boost ^= 1;
		g_Var.Alarm_Flicker.Bits.VBus ^= 1;
		g_Var.Alarm_Flicker.Bits.Bat ^= 1;
		g_Var.Alarm_Flicker.Bits.Proc ^= 1;
//		g_Var.Alarm_Flicker.Bits.Func ^= 1;
		g_Var.Alarm_Flicker.Bits.Wifi ^= 1;
	}

	if (g_Var.nTmr++ >= 100)
	{
		g_Var.nTmr = 0;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case SW_IO0_Pin:
		g_Var.SW_State = SW_PWR;
		break;

	case SW_IO1_Pin:
		g_Var.SW_State = SW_UP;
		break;

	case SW_IO2_Pin:
		g_Var.SW_State = SW_DOWN;
		break;

	case SW_IO3_Pin:
		g_Var.SW_State = SW_DIR;
		break;

	case SW_IO4_Pin:
		g_Var.SW_State = SW_SET;
		break;

	case SW_IO5_Pin:
		g_Var.SW_State = SW_SAVE;
		break;

	default:
		break;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart->Instance == USART1)
	{
		ReceiveCombuf_DEBUG(&(g_Var.Uart_recvBuf), 1);
		if(g_Var.Uart_echoState)	HAL_UART_Transmit(huart, &(g_Var.Uart_recvBuf), 1, 10);	// Echo
		//HAL_UART_Transmit(&huart3, &(g_Var.Uart_recvBuf), 1, 10);	// Echo
		HAL_UART_Receive_IT(huart, &(g_Var.Uart_recvBuf), 1);
		return;
	}

	if(huart->Instance == USART2)
	{
		ReceiveCombuf_WIFI(&(g_Var.Uart_recvBuf), 1);
		HAL_UART_Receive_IT(huart, &(g_Var.Uart_recvBuf), 1);
		return;
	}
}

// I2C
uint8_t CheckBusyI2C(I2C_HandleTypeDef* hi2c, uint16_t DevAddress)
{
	uint8_t i = TIME_OUT_I2C;
	while (i--)
	{
		if (HAL_I2C_IsDeviceReady(hi2c, DevAddress, 10, 10) == HAL_OK) return 1;
	}
	return 0;
}

void ReadsEEP(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen)
{
    HAL_I2C_Mem_Read(&hi2c1, SLAVE_ADD_EEP, wAdd, I2C_MEMADD_SIZE_16BIT, cBuf, nLen, 100);
}

void WritesEEP(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen)
{
	int i;
#if 0
	CheckBusyI2C(&hi2c1, SLAVE_ADD_EEP);
    HAL_I2C_Mem_Write(&hi2c1, SLAVE_ADD_EEP, wAdd, I2C_MEMADD_SIZE_16BIT, cBuf, nLen, 100);
#else
    for (i=0; i<nLen; i++)
    {
    	//if(CheckBusyI2C(&hi2c1, SLAVE_ADD_EEP))
    	CheckBusyI2C(&hi2c1, SLAVE_ADD_EEP);
		HAL_I2C_Mem_Write(&hi2c1, SLAVE_ADD_EEP, wAdd + i, I2C_MEMADD_SIZE_16BIT, &cBuf[i], 1, 10);
    }
#endif
}

void ReadTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen)
{
	HAL_I2C_Mem_Read(&hi2c3, SLAVE_DEV_ADD_TPL1401_READ, wAdd, I2C_MEMADD_SIZE_8BIT, cBuf, nLen, 100);
}

void WriteTPL1401(uint16_t wAdd, uint8_t *cBuf, uint16_t nLen)
{
	//if(CheckBusyI2C(&hi2c3, SLAVE_DEV_ADD_TPL1401_WRITE))
	CheckBusyI2C(&hi2c3, SLAVE_DEV_ADD_TPL1401_WRITE);
	HAL_I2C_Mem_Write(&hi2c3, SLAVE_DEV_ADD_TPL1401_WRITE, wAdd, I2C_MEMADD_SIZE_8BIT, cBuf, nLen, 100);
	//HAL_I2C_Mem_Write(&hi2c3, SLAVE_DEV_ADD_TPL1401_BROADCAST, wAdd, I2C_MEMADD_SIZE_8BIT, cBuf, nLen, 100);
}


// COMMUNICATION
uint16_t Search_StrCmd(const char** arr, char* target, uint16_t arrlen)
{
	uint16_t ret = 99;

	for(int i = 0; i < arrlen; i++)
	{
		if(strcmp(target, arr[i]) == 0)
		{
			ret = i;
			break;
		}
	}

	return ret;
}


/*
  	 DEBUG Serial
*/
void ReceiveCombuf_DEBUG(uint8_t* Buf, uint32_t Len)
{
	for(int i = 0; i < Len; i++)
	{
		RingBuf_Insert(&RingBuf_DEBUG, Buf[i]);
	}
}

void ProcessCombuf_DEBUG(void)
{
	uint16_t bufsize = RingBuf_Getdatalength(&RingBuf_DEBUG);

	// 버퍼에 데이터가 없는 경우
	if(!bufsize) return;

	size_t nb = 0, nr = 0;
	uint16_t index = 0;
	uint8_t * savebuf = 0;
	char** strarr0 = 0;
	char** strarr1 = 0;

	// 버퍼 메모리 할당
	savebuf = (uint8_t *)malloc(bufsize * sizeof(uint8_t));
	if(!savebuf) return;

	// 버퍼 메모리에 메모리 복사 및 '\n'의 개수 조사
	index = RingBuf_Gettail(&RingBuf_DEBUG);
	for(int i = 0; i < bufsize; i++)
	{
		savebuf[i] = RingBuf_Getdata(&RingBuf_DEBUG, index);
		nr += (savebuf[i] == '\n');
		//if(savebuf[i] == '\n') nr++;
		index++;
		index &= (RxBufSIZE - 1);
	}

	// '\r'이 단 한개도 없으면 무시
	if(!nr)
	{
		free(savebuf);
		return;
	}

	strarr0 = strsplit((char*)savebuf, "\n");
	strarr1 = strsplit_count((char*)strarr0[0], ",", &nb);

	//printf("%d\r\n", nb);
	//printf("%s\r\n", strarr0[0]);

	const char* arrcmd[6] =
	{
			"DATA_LEN\r",
			"EEP_READ\r",
			"EEP_CLEAR\r",
			"ECHO_OFF\r",
			"ECHO_ON\r",
			""
	};

	switch(Search_StrCmd(arrcmd, strarr1[0], 9))
	{
	case 0:	// DATA_LEN
	{
		uint8_t DTLen = (uint8_t)atoi(strarr1[1]);
		SetDataLength(DTLen);
		printf("\r\nComplete to save Data length.\r\n");
		printf("ODAS>");
	}
		break;
	case 1:	// EEP_READ
	{
		uint16_t startaddr = EEP_ADD_DT + sizeof(stDT_SAVE) + sizeof(uint8_t);
		char str_sign[2] = { ' ', '-' };

		printf("\r\n");
		printf("Number,\t Wavelength,\t OptPower,\t Attenuation,\r\n");
		for(int i = 0; i <= MAX_DISPLAYBUF_INDEX_EEP; i++)
		{
			ReadsEEP(startaddr + sizeof(STC_DT_DISPLAY_BUF) * i,
							(uint8_t *)&(stDT_DISPLAY), sizeof(STC_DT_DISPLAY_BUF));
			printf("%d, \t", i + 1);
			printf("%c%d.%02d, \t", str_sign[stDT_DISPLAY.Wavelen < 0],
					(uint16_t)abs(stDT_DISPLAY.Wavelen / DIV_WAVE),
					(uint16_t)(stDT_DISPLAY.Wavelen > 0 ?
							stDT_DISPLAY.Wavelen % DIV_WAVE :
							(stDT_DISPLAY.Wavelen % DIV_WAVE) * (-1)));
			printf("%c%d.%02d, \t", str_sign[stDT_DISPLAY.OptPower < 0],
					abs(stDT_DISPLAY.OptPower / DIV_OPT),
					stDT_DISPLAY.OptPower > 0 ?
							stDT_DISPLAY.OptPower % DIV_OPT :
							(stDT_DISPLAY.OptPower % DIV_OPT) * (-1));
			printf("%c%d.%02d, \r\n", str_sign[stDT_DISPLAY.Attenuation < 0],
					abs(stDT_DISPLAY.Attenuation / DIV_OPT),
					stDT_DISPLAY.Attenuation > 0 ?
							stDT_DISPLAY.Attenuation % DIV_OPT :
							(stDT_DISPLAY.Attenuation % DIV_OPT) * (-1));
		}
		printf("ODAS>");
	}
		break;
	case 2:	// EEP_CLEAR
	{
		ClearDataToEEPROM();
		printf("\r\nComplete to clear EEPROM.\r\n");
		printf("ODAS>");
	}
		break;
	case 3:	// ECHO_OFF
	{
		g_Var.Uart_echoState = 0;
		printf("\r\n");
		printf("ODAS>");
	}
		break;
	case 4:	// ECHO_ON
	{
		g_Var.Uart_echoState = 1;
		printf("\r\n");
		printf("ODAS>");
	}
		break;
	default: // Invalid command
		printf("\r\n");
		printf("ODAS>");
		break;

	}


	RingBuf_Delete(&RingBuf_DEBUG, strlen(strarr0[0]) + 1);

	free(strarr1);
	free(strarr0);
	free(savebuf);

	g_Var.Cnt_Auto_Off = 0;
}

/*
 	 USB to Serial
*/
void ReceiveCombuf_USB(uint8_t* Buf, uint32_t Len)
{
	for(int i = 0; i < Len; i++)
	{
		RingBuf_Insert(&RingBuf_USB, Buf[i]);
	}
}

void ProcessCombuf_USB(void)
{
	uint16_t bufsize = RingBuf_Getdatalength(&RingBuf_USB);

	// 버퍼에 데이터가 없는 경우
	if(!bufsize) return;

	size_t nb = 0, nr = 0;
	uint16_t index = 0;
	uint8_t * savebuf = 0;
	char** strarr0 = 0;
	char** strarr1 = 0;


	// 버퍼 메모리 할당
	savebuf = (uint8_t *)malloc(bufsize * sizeof(uint8_t));
	if(!savebuf) return;

	// 버퍼 메모리에 메모리 복사 및 '\n'의 개수 조사
	index = RingBuf_Gettail(&RingBuf_USB);
	for(int i = 0; i < bufsize; i++)
	{
		savebuf[i] = RingBuf_Getdata(&RingBuf_USB, index);
		nr += (savebuf[i] == '\n');
		//if(savebuf[i] == '\n') nr++;
		index++;
		index &= (RxBufSIZE - 1);
	}

	// '\r'이 단 한개도 없으면 무시
	if(!nr)
	{
		free(savebuf);
		return;
	}

	strarr0 = strsplit((char*)savebuf, "\n");
	strarr1 = strsplit_count((char*)strarr0[0], ",", &nb);

	const char* arrcmd[1] =
	{
			"USB"
	};

	switch(Search_StrCmd(arrcmd, strarr1[0], 1))
	{
	case 0:	// USB
	{
		uint16_t cmd = 0;
		memcpy(&cmd, strarr1[1], sizeof(cmd));
		ProcessComCmd_USB(cmd, (const char**)strarr1);
	}
		break;

	default: // Invalid command
		break;

	}


	RingBuf_Delete(&RingBuf_USB, strlen(strarr0[0]) + 1);

	free(strarr1);
	free(strarr0);
	free(savebuf);

	//g_Var.Cnt_Auto_Off = 0;
}

void ProcessComCmd_USB(uint16_t cmd, const char** strarr)
{
	switch(cmd)
	{
	case COMM_CMD_RESET:
		break;

	case COMM_CMD_SAVE_VERSION:
	{
		uint16_t cmd = COMM_CMD_SAVE_VERSION;
		SaveVersionInfo((uint8_t*)strarr[2]);

		if(CDC_Transmit_FS((uint8_t*)&cmd, sizeof(cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_VERSION:
	{
		LoadVersionInfo();

		if(CDC_Transmit_FS((uint8_t*)&stVERSIONINFO, sizeof(stVERSIONINFO)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_STATUS:
	{
		SendUIStatus_USB();
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		uint32_t wave = (uint32_t)atoi(strarr[3]);

		stDT_SAVE[0].Wavelen[index] = wave;
		WritesEEP(EEP_ADD_DT + (sizeof(uint32_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Wavelen[index]), sizeof(uint32_t));

		stWAVE.Cmd = COMM_CMD_DOWNLINK_UPLOAD_WAVE;

		if(CDC_Transmit_FS((uint8_t*)&(stWAVE.Cmd), sizeof(stWAVE.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t offset_A = (int16_t)atoi(strarr[3]);

		stDT_SAVE[0].Offset_A[index] = offset_A;
		WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE[0].Wavelen) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Offset_A[index]), sizeof(int16_t));

		stOFFSET_A.Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A;

		if(CDC_Transmit_FS((uint8_t*)&(stOFFSET_A.Cmd), sizeof(stOFFSET_A.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t offset_B = (int16_t)atoi(strarr[3]);

		stDT_SAVE[0].Offset_B[index] = offset_B;
		WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE[0].Wavelen) + sizeof(stDT_SAVE[0].Offset_A) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Offset_B[index]), sizeof(int16_t));

		stOFFSET_B.Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B;

		if(CDC_Transmit_FS((uint8_t*)&(stOFFSET_B.Cmd), sizeof(stOFFSET_B.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t limit = (int16_t)atoi(strarr[3]);
		uint8_t enable = (uint8_t)atoi(strarr[4]);

		stDT_SAVE[0].Alarm_Limit[index] = limit;
		stDT_SAVE[0].Alarm_Enable[index] = enable;
		WritesEEP(EEP_ADD_DT +
				sizeof(stDT_SAVE[0].Wavelen) +
				sizeof(stDT_SAVE[0].Offset_A) +
				sizeof(stDT_SAVE[0].Offset_B) +
				sizeof(stDT_SAVE[0].Attenuation) +
				sizeof(stDT_SAVE[0].Ref_Wavelen) +
				sizeof(stDT_SAVE[0].Boundary_Offset) +
				(sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Alarm_Limit[index]), sizeof(int16_t));

		WritesEEP(EEP_ADD_DT +
				sizeof(stDT_SAVE[0].Wavelen) +
				sizeof(stDT_SAVE[0].Offset_A) +
				sizeof(stDT_SAVE[0].Offset_B) +
				sizeof(stDT_SAVE[0].Attenuation) +
				sizeof(stDT_SAVE[0].Ref_Wavelen) +
				sizeof(stDT_SAVE[0].Boundary_Offset) +
				sizeof(stDT_SAVE[0].Alarm_Limit) +
				(sizeof(uint8_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Alarm_Enable[index]), sizeof(uint8_t));

		stALARM.Cmd = COMM_CMD_DOWNLINK_UPLOAD_ALARM;

		if(CDC_Transmit_FS((uint8_t*)&(stALARM.Cmd), sizeof(stALARM.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_WAVE:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stWAVE.Cmd = COMM_CMD_DOWNLINK_LOAD_WAVE;
		stWAVE.Index = index;
		stWAVE.Wavelen = stDT_SAVE[0].Wavelen[index];

		if(CDC_Transmit_FS((uint8_t*)&stWAVE, sizeof(stWAVE)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stOFFSET_A.Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_A;
		stOFFSET_A.Index = index;
		stOFFSET_A.Offset = stDT_SAVE[0].Offset_A[index];

		if(CDC_Transmit_FS((uint8_t*)&stOFFSET_A, sizeof(stOFFSET_A)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stOFFSET_B.Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_B;
		stOFFSET_B.Index = index;
		stOFFSET_B.Offset = stDT_SAVE[0].Offset_B[index];

		if(CDC_Transmit_FS((uint8_t*)&stOFFSET_B, sizeof(stOFFSET_B)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_ALARM:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stALARM.Cmd = COMM_CMD_DOWNLINK_LOAD_ALARM;
		stALARM.Index = index;
		stALARM.Limit = stDT_SAVE[0].Alarm_Limit[index];
		stALARM.Enable = stDT_SAVE[0].Alarm_Enable[index];

		if(CDC_Transmit_FS((uint8_t*)&stALARM, sizeof(stALARM)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
	{
		int16_t val = (int16_t)atoi(strarr[2]);

		SetAlarmLimit(0, val);

		SendUIStatus_USB();

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
	{
		int32_t val = (int32_t)atoi(strarr[2]);

		SetBoundaryOffset(0, val);

		SendUIStatus_USB();

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_WAVE:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		uint32_t wave = (uint32_t)atoi(strarr[3]);

		stDT_SAVE[1].Wavelen[index] = wave;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + (sizeof(uint32_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Wavelen[index]), sizeof(uint32_t));

		stWAVE.Cmd = COMM_CMD_UPLINK_UPLOAD_WAVE;

		if(CDC_Transmit_FS((uint8_t*)&(stWAVE.Cmd), sizeof(stWAVE.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t offset_A = (int16_t)atoi(strarr[3]);

		stDT_SAVE[1].Offset_A[index] = offset_A;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + sizeof(stDT_SAVE[1].Wavelen) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Offset_A[index]), sizeof(int16_t));

		stOFFSET_A.Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_A;

		if(CDC_Transmit_FS((uint8_t*)&(stOFFSET_A.Cmd), sizeof(stOFFSET_A.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t offset_B = (int16_t)atoi(strarr[3]);

		stDT_SAVE[1].Offset_B[index] = offset_B;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + sizeof(stDT_SAVE[1].Wavelen) + sizeof(stDT_SAVE[1].Offset_A) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Offset_B[index]), sizeof(int16_t));

		stOFFSET_B.Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_B;

		if(CDC_Transmit_FS((uint8_t*)&(stOFFSET_B.Cmd), sizeof(stOFFSET_B.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_ALARM:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		int16_t limit = (int16_t)atoi(strarr[3]);
		uint8_t enable = (uint8_t)atoi(strarr[4]);

		stDT_SAVE[1].Alarm_Limit[index] = limit;
		stDT_SAVE[1].Alarm_Enable[index] = enable;
		WritesEEP(EEP_ADD_DT +
				sizeof(STC_DT_SAVE_BUF) +
				sizeof(stDT_SAVE[1].Wavelen) +
				sizeof(stDT_SAVE[1].Offset_A) +
				sizeof(stDT_SAVE[1].Offset_B) +
				sizeof(stDT_SAVE[1].Attenuation) +
				sizeof(stDT_SAVE[1].Ref_Wavelen) +
				sizeof(stDT_SAVE[1].Boundary_Offset) +
				(sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Alarm_Limit[index]), sizeof(int16_t));

		WritesEEP(EEP_ADD_DT +
				sizeof(STC_DT_SAVE_BUF) +
				sizeof(stDT_SAVE[1].Wavelen) +
				sizeof(stDT_SAVE[1].Offset_A) +
				sizeof(stDT_SAVE[1].Offset_B) +
				sizeof(stDT_SAVE[1].Attenuation) +
				sizeof(stDT_SAVE[1].Ref_Wavelen) +
				sizeof(stDT_SAVE[1].Boundary_Offset) +
				sizeof(stDT_SAVE[1].Alarm_Limit) +
				(sizeof(uint8_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Alarm_Enable[index]), sizeof(uint8_t));

		stALARM.Cmd = COMM_CMD_UPLINK_UPLOAD_ALARM;

		if(CDC_Transmit_FS((uint8_t*)&(stALARM.Cmd), sizeof(stALARM.Cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_WAVE:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stWAVE.Cmd = COMM_CMD_UPLINK_LOAD_WAVE;
		stWAVE.Index = index;
		stWAVE.Wavelen = stDT_SAVE[1].Wavelen[index];

		if(CDC_Transmit_FS((uint8_t*)&stWAVE, sizeof(stWAVE)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_OFFSET_A:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stOFFSET_A.Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_A;
		stOFFSET_A.Index = index;
		stOFFSET_A.Offset = stDT_SAVE[1].Offset_A[index];

		if(CDC_Transmit_FS((uint8_t*)&stOFFSET_A, sizeof(stOFFSET_A)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_OFFSET_B:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stOFFSET_B.Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_B;
		stOFFSET_B.Index = index;
		stOFFSET_B.Offset = stDT_SAVE[1].Offset_B[index];

		if(CDC_Transmit_FS((uint8_t*)&stOFFSET_B, sizeof(stOFFSET_B)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_ALARM:
	{
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stALARM.Cmd = COMM_CMD_UPLINK_LOAD_ALARM;
		stALARM.Index = index;
		stALARM.Limit = stDT_SAVE[1].Alarm_Limit[index];
		stALARM.Enable = stDT_SAVE[1].Alarm_Enable[index];

		if(CDC_Transmit_FS((uint8_t*)&stALARM, sizeof(stALARM)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
	{
		int16_t val = (int16_t)atoi(strarr[2]);

		SetAlarmLimit(1, val);

		SendUIStatus_USB();

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
	{
		int32_t val = (int32_t)atoi(strarr[2]);

		SetBoundaryOffset(1, val);

		SendUIStatus_USB();

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_DATA:
	{
		uint16_t startaddr = EEP_ADD_DT + sizeof(stDT_SAVE) + sizeof(uint8_t);
		uint8_t index = (uint8_t)atoi(strarr[2]);
		stDATA.Cmd = COMM_CMD_LOAD_DATA;

		ReadsEEP(startaddr + sizeof(STC_DT_DISPLAY_BUF) * index, (uint8_t *)&stDT_DISPLAY, sizeof(STC_DT_DISPLAY_BUF));
		stDATA.Index = index;
		stDATA.Wavelen = stDT_DISPLAY.Wavelen;
		stDATA.OptPower = stDT_DISPLAY.OptPower;
		stDATA.Attenuation = stDT_DISPLAY.Attenuation;
		stDATA.OptSw = stDT_DISPLAY.OptSw;

		if(CDC_Transmit_FS((uint8_t*)&stDATA, sizeof(STC_DATA_BUF)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_OPTSW_DOWN:
	{
		g_Var.CurrOptSw = 0;
		SetOptSW(g_Var.CurrOptSw);
		SendUIStatus_USB();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_OPTSW_UP:
	{
		g_Var.CurrOptSw = 1;
		SetOptSW(g_Var.CurrOptSw);
		SendUIStatus_USB();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_CHANNEL:
	{
		uint8_t ch = (uint8_t)atoi(strarr[2]);
		if(ch < g_Var.EEP_stDT_Length)	g_Var.CurrSelChannel = ch;
		SendUIStatus_USB();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_MAXDATALEN:
	{
		uint8_t len = (uint8_t)atoi(strarr[2]);
		uint16_t cmd = COMM_CMD_SET_MAXDATALEN;

		SetDataLength(len);

		if(CDC_Transmit_FS((uint8_t*)&cmd, sizeof(cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_ATT:
	{
		int16_t att = (int16_t)atoi(strarr[2]);
		SetAttenuation(att);
		SendUIStatus_USB();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_RESET_ATT:
	{
		ResetAttenuation();
		SendUIStatus_USB();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SAVE:
	{
		SaveDataToEEPROM();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SAVE_NETWORKINFO:
	{
		uint16_t cmd = COMM_CMD_SAVE_NETWORKINFO;
		uint8_t ip = (uint8_t)atoi(strarr[4]);

		SaveNetworkInfo((uint8_t*)strarr[2], (uint8_t*)strarr[3], ip);

		if(CDC_Transmit_FS((uint8_t*)&cmd, sizeof(cmd)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_NETWORKINFO:
	{
		LoadNetworkInfo();

		if(CDC_Transmit_FS((uint8_t*)&stNETWORK, sizeof(stNETWORK)) != HAL_OK)
		{

		}

		g_Var.Cnt_Auto_Off = 0;
	}
			break;

	default:
		break;
	}

}

void SendUIStatus_USB(void)
{
	memcpy(stUI_STATUS.Optpower, stDT_REALTIME.OptPower, sizeof(stUI_STATUS.Optpower));
	stUI_STATUS.Cmd = COMM_CMD_STATUS;
	stUI_STATUS.Attenuation = stDT_SAVE[0].Attenuation;
	stUI_STATUS.Ref_Wavelen = stDT_SAVE[0].Ref_Wavelen;
	stUI_STATUS.Boundary_Offset[0] = stDT_SAVE[0].Boundary_Offset;
	stUI_STATUS.Boundary_Offset[1] = stDT_SAVE[1].Boundary_Offset;
	stUI_STATUS.OptPower_Alarm_Limit[0] = stDT_SAVE[0].Alarm_Temperallimit;
	stUI_STATUS.OptPower_Alarm_Limit[1] = stDT_SAVE[1].Alarm_Temperallimit;
	stUI_STATUS.CurrSelChannel = g_Var.CurrSelChannel;
	stUI_STATUS.Channellen = g_Var.EEP_stDT_Length;
	stUI_STATUS.CurrOptSW = g_Var.CurrOptSw;
	stUI_STATUS.Bat_Capacity = stSTATE.Bat_Capacity;
	stUI_STATUS.IN_Power = stSTATE.IN_Power;
	stUI_STATUS.Wifi_State = g_Var.Wifi_State;
	stUI_STATUS.Alarm.Bits.Proc = g_Var.Alarm_State.Bits.Proc;
	stUI_STATUS.Alarm.Bits.Func = g_Var.Alarm_State.Bits.Func;
	stUI_STATUS.Alarm.Bits.Boost = g_Var.Alarm_State.Bits.Boost;
	stUI_STATUS.Alarm.Bits.VBus = g_Var.Alarm_State.Bits.VBus;
	stUI_STATUS.Alarm.Bits.Bat = g_Var.Alarm_State.Bits.Bat;

	if(CDC_Transmit_FS((uint8_t*)&stUI_STATUS, sizeof(stUI_STATUS)) != HAL_OK)
	{

	}
}



/*
 	 WIFI
*/
void ReceiveCombuf_WIFI(uint8_t* Buf, uint32_t Len)
{
	for(int i = 0; i < Len; i++)
	{
		RingBuf_Insert(&RingBuf_WIFI, Buf[i]);
	}
}

void ProcessCombuf_WIFI(void)
{
	uint16_t bufsize = RingBuf_Getdatalength(&RingBuf_WIFI);

	// 버퍼에 데이터가 없는 경우
	if(!bufsize) return;

	size_t nb = 0, nr = 0;
	uint16_t index = 0;
	uint8_t * savebuf = 0;
	char** strarr0 = 0;
	char** strarr1 = 0;
	char** strarr2 = 0;
	char** strarr3 = 0;


	// 버퍼 메모리 할당
	savebuf = (uint8_t *)malloc(bufsize * sizeof(uint8_t));
	if(!savebuf) return;

	// 버퍼 메모리에 메모리 복사 및 '\n'의 개수 조사
	index = RingBuf_Gettail(&RingBuf_WIFI);
	for(int i = 0; i < bufsize; i++)
	{
		savebuf[i] = RingBuf_Getdata(&RingBuf_WIFI, index);
		nr += (savebuf[i] == '\n');
		//if(savebuf[i] == '\n') nr++;
		index++;
		index &= (RxBufSIZE - 1);
	}

	// '\r'이 단 한개도 없으면 무시
	if(!nr)
	{
		free(savebuf);
		return;
	}

	strarr0 = strsplit((char*)savebuf, "\n");
	strarr1 = strsplit_count((char*)strarr0[0], ":", &nb);
	strarr2 = strsplit_count((char*)strarr1[0], ",", &nb);

	// Debug
	//if(strarr0) printf("%s\n", strarr0[0]);

	const char* arrcmd[3] =
	{
		"+IPD",
		"OK\r",
		"ERROR\r"
	};

	switch(Search_StrCmd(arrcmd, strarr2[0], 3))
	{
	case 0:	// TCP
	{
		uint16_t cmd = 0;
		uint8_t link_id = atoi(strarr2[1]);
		strarr3 = strsplit((char*)strarr1[1], ",");
		memcpy(&cmd, strarr3[0], sizeof(cmd));
		ProcessComCmd_WIFI(cmd, link_id, (const char**)strarr3);
		free(strarr3);
	}
		break;

	case 1:	// Send data
	{
		ProcessComSend_WIFI();
	}
		break;

	case 2:	// Error
	{
		//printf("ESP8266 WIFI Communication Error!!\r\n");
	}
		break;

	default: // Invalid command
		break;

	}

	RingBuf_Delete(&RingBuf_WIFI, strlen(strarr0[0]) + 1);

	free(strarr2);
	free(strarr1);
	free(strarr0);
	free(savebuf);

	//g_Var.Cnt_Auto_Off = 0;
}

void ProcessComCmd_WIFI(uint16_t cmd, uint8_t id, const char** strarr)
{
	switch(cmd)
	{
	case COMM_CMD_RESET:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_RESET;
	}
		break;

	case COMM_CMD_SAVE_VERSION:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SAVE_VERSION;

		stVERSIONINFO.Cmd = COMM_CMD_SAVE_VERSION;
		SaveVersionInfo((uint8_t*)strarr[1]);

		esp8266_ready_send_TCP(id, sizeof(stVERSIONINFO.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_VERSION:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_LOAD_VERSION;

		LoadVersionInfo();

		esp8266_ready_send_TCP(id, sizeof(stVERSIONINFO));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_STATUS:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_STATUS;

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_WAVE;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		uint32_t wave = (uint32_t)atoi(strarr[2]);

		stDT_SAVE[0].Wavelen[index] = wave;
		WritesEEP(EEP_ADD_DT + (sizeof(uint32_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Wavelen[index]), sizeof(uint32_t));

		stWAVE.Cmd = COMM_CMD_DOWNLINK_UPLOAD_WAVE;

		esp8266_ready_send_TCP(id, sizeof(stWAVE.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_OFFSET_A;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t offset_A = (int16_t)atoi(strarr[2]);

		stDT_SAVE[0].Offset_A[index] = offset_A;
		WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE[0].Wavelen) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Offset_A[index]), sizeof(int16_t));

		stOFFSET_A.Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A;

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_A.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_OFFSET_B;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t offset_B = (int16_t)atoi(strarr[2]);

		stDT_SAVE[0].Offset_B[index] = offset_B;
		WritesEEP(EEP_ADD_DT + sizeof(stDT_SAVE[0].Wavelen) + sizeof(stDT_SAVE[0].Offset_A) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Offset_B[index]), sizeof(int16_t));

		stOFFSET_B.Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B;

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_B.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_ALARM;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t limit = (int16_t)atoi(strarr[2]);
		uint8_t enable = (uint8_t)atoi(strarr[3]);

		stDT_SAVE[0].Alarm_Limit[index] = limit;
		stDT_SAVE[0].Alarm_Enable[index] = enable;
		WritesEEP(EEP_ADD_DT +
				sizeof(stDT_SAVE[0].Wavelen) +
				sizeof(stDT_SAVE[0].Offset_A) +
				sizeof(stDT_SAVE[0].Offset_B) +
				sizeof(stDT_SAVE[0].Attenuation) +
				sizeof(stDT_SAVE[0].Ref_Wavelen) +
				sizeof(stDT_SAVE[0].Boundary_Offset) +
				(sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Alarm_Limit[index]), sizeof(int16_t));

		WritesEEP(EEP_ADD_DT +
				sizeof(stDT_SAVE[0].Wavelen) +
				sizeof(stDT_SAVE[0].Offset_A) +
				sizeof(stDT_SAVE[0].Offset_B) +
				sizeof(stDT_SAVE[0].Attenuation) +
				sizeof(stDT_SAVE[0].Ref_Wavelen) +
				sizeof(stDT_SAVE[0].Boundary_Offset) +
				sizeof(stDT_SAVE[0].Alarm_Limit) +
				(sizeof(uint8_t) * index),
				(uint8_t *)&(stDT_SAVE[0].Alarm_Enable[index]), sizeof(uint8_t));

		stALARM.Cmd = COMM_CMD_DOWNLINK_UPLOAD_ALARM;

		esp8266_ready_send_TCP(id, sizeof(stALARM.Cmd));

		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_WAVE:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_WAVE;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stWAVE.Cmd = COMM_CMD_DOWNLINK_LOAD_WAVE;
		stWAVE.Index = index;
		stWAVE.Wavelen = stDT_SAVE[0].Wavelen[index];

		esp8266_ready_send_TCP(id, sizeof(stWAVE));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_OFFSET_A;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stOFFSET_A.Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_A;
		stOFFSET_A.Index = index;
		stOFFSET_A.Offset = stDT_SAVE[0].Offset_A[index];

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_A));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_OFFSET_B;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stOFFSET_B.Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_B;
		stOFFSET_B.Index = index;
		stOFFSET_B.Offset = stDT_SAVE[0].Offset_B[index];

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_B));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_DOWNLINK_LOAD_ALARM:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_ALARM;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stALARM.Cmd = COMM_CMD_DOWNLINK_LOAD_ALARM;
		stALARM.Index = index;
		stALARM.Limit = stDT_SAVE[0].Alarm_Limit[index];
		stALARM.Enable = stDT_SAVE[0].Alarm_Enable[index];

		esp8266_ready_send_TCP(id, sizeof(stALARM));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_SET_ALARM_LIMIT;

		int16_t val = (int16_t)atoi(strarr[1]);

		SetAlarmLimit(0, val);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_DOWNLINK_SET_BOUNDARY_OFFSET;

		int32_t val = (int32_t)atoi(strarr[1]);
		SetBoundaryOffset(0, val);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_UPLINK_UPLOAD_WAVE:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_WAVE;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		uint32_t wave = (uint32_t)atoi(strarr[2]);

		stDT_SAVE[1].Wavelen[index] = wave;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + (sizeof(uint32_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Wavelen[index]), sizeof(uint32_t));

		stWAVE.Cmd = COMM_CMD_UPLINK_UPLOAD_WAVE;

		esp8266_ready_send_TCP(id, sizeof(stWAVE.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_OFFSET_A;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t offset_A = (int16_t)atoi(strarr[2]);

		stDT_SAVE[1].Offset_A[index] = offset_A;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + sizeof(stDT_SAVE[1].Wavelen) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Offset_A[index]), sizeof(int16_t));

		stOFFSET_A.Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_A;

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_A.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_OFFSET_B;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t offset_B = (int16_t)atoi(strarr[2]);

		stDT_SAVE[1].Offset_B[index] = offset_B;
		WritesEEP(EEP_ADD_DT + sizeof(STC_DT_SAVE_BUF) + sizeof(stDT_SAVE[1].Wavelen) + sizeof(stDT_SAVE[1].Offset_A) + (sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Offset_B[index]), sizeof(int16_t));

		stOFFSET_B.Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_B;

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_B.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_UPLINK_UPLOAD_ALARM:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_ALARM;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		int16_t limit = (int16_t)atoi(strarr[2]);
		uint8_t enable = (uint8_t)atoi(strarr[3]);

		stDT_SAVE[1].Alarm_Limit[index] = limit;
		stDT_SAVE[1].Alarm_Enable[index] = enable;
		WritesEEP(EEP_ADD_DT +
				sizeof(STC_DT_SAVE_BUF) +
				sizeof(stDT_SAVE[1].Wavelen) +
				sizeof(stDT_SAVE[1].Offset_A) +
				sizeof(stDT_SAVE[1].Offset_B) +
				sizeof(stDT_SAVE[1].Attenuation) +
				sizeof(stDT_SAVE[1].Ref_Wavelen) +
				sizeof(stDT_SAVE[1].Boundary_Offset) +
				(sizeof(int16_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Alarm_Limit[index]), sizeof(int16_t));

		WritesEEP(EEP_ADD_DT +
				sizeof(STC_DT_SAVE_BUF) +
				sizeof(stDT_SAVE[1].Wavelen) +
				sizeof(stDT_SAVE[1].Offset_A) +
				sizeof(stDT_SAVE[1].Offset_B) +
				sizeof(stDT_SAVE[1].Attenuation) +
				sizeof(stDT_SAVE[1].Ref_Wavelen) +
				sizeof(stDT_SAVE[1].Boundary_Offset) +
				sizeof(stDT_SAVE[1].Alarm_Limit) +
				(sizeof(uint8_t) * index),
				(uint8_t *)&(stDT_SAVE[1].Alarm_Enable[index]), sizeof(uint8_t));

		stALARM.Cmd = COMM_CMD_UPLINK_UPLOAD_ALARM;

		esp8266_ready_send_TCP(id, sizeof(stALARM.Cmd));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_WAVE:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_WAVE;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stWAVE.Cmd = COMM_CMD_UPLINK_LOAD_WAVE;
		stWAVE.Index = index;
		stWAVE.Wavelen = stDT_SAVE[1].Wavelen[index];

		esp8266_ready_send_TCP(id, sizeof(stWAVE));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_OFFSET_A:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_OFFSET_A;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stOFFSET_A.Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_A;
		stOFFSET_A.Index = index;
		stOFFSET_A.Offset = stDT_SAVE[1].Offset_A[index];

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_A));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_LOAD_OFFSET_B:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_OFFSET_B;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stOFFSET_B.Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_B;
		stOFFSET_B.Index = index;
		stOFFSET_B.Offset = stDT_SAVE[1].Offset_B[index];

		esp8266_ready_send_TCP(id, sizeof(stOFFSET_B));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_UPLINK_LOAD_ALARM:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_ALARM;

		uint8_t index = (uint8_t)atoi(strarr[1]);
		stALARM.Cmd = COMM_CMD_UPLINK_LOAD_ALARM;
		stALARM.Index = index;
		stALARM.Limit = stDT_SAVE[1].Alarm_Limit[index];
		stALARM.Enable = stDT_SAVE[1].Alarm_Enable[index];

		esp8266_ready_send_TCP(id, sizeof(stALARM));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_SET_ALARM_LIMIT;

		int16_t val = (int16_t)atoi(strarr[1]);

		SetAlarmLimit(1, val);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;


	case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_UPLINK_SET_BOUNDARY_OFFSET;

		int32_t val = (int32_t)atoi(strarr[1]);
		SetBoundaryOffset(1, val);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_DATA:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_LOAD_DATA;

		uint16_t startaddr = EEP_ADD_DT + sizeof(stDT_SAVE) + sizeof(uint8_t);
		uint8_t index = (uint8_t)atoi(strarr[1]);
		stDATA.Cmd = COMM_CMD_LOAD_DATA;

		ReadsEEP(startaddr + sizeof(STC_DT_DISPLAY_BUF) * index, (uint8_t *)&stDT_DISPLAY, sizeof(STC_DT_DISPLAY_BUF));
		stDATA.Index = index;
		stDATA.Wavelen = stDT_DISPLAY.Wavelen;
		stDATA.OptPower = stDT_DISPLAY.OptPower;
		stDATA.Attenuation = stDT_DISPLAY.Attenuation;
		stDATA.OptSw = stDT_DISPLAY.OptSw;

		esp8266_ready_send_TCP(id, sizeof(STC_DATA_BUF));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_OPTSW_DOWN:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_OPTSW_DOWN;

		g_Var.CurrOptSw = 0;
		SetOptSW(g_Var.CurrOptSw);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_OPTSW_UP:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_OPTSW_UP;

		g_Var.CurrOptSw = 1;
		SetOptSW(g_Var.CurrOptSw);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_CHANNEL:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SET_CHANNEL;

		uint8_t ch = (uint8_t)atoi(strarr[1]);
		if(ch < g_Var.EEP_stDT_Length)	g_Var.CurrSelChannel = ch;

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_MAXDATALEN:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SET_MAXDATALEN;

		uint8_t len = (uint8_t)atoi(strarr[1]);
		SetDataLength(len);

		esp8266_ready_send_TCP(id, sizeof(g_esp8266.data_send_status));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SET_ATT:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SET_ATT;

		int16_t att = (int16_t)atoi(strarr[1]);
		SetAttenuation(att);

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_RESET_ATT:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_RESET_ATT;

		ResetAttenuation();

		esp8266_ready_send_TCP(id, sizeof(stUI_STATUS));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SAVE:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SAVE;

		SaveDataToEEPROM();
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_SAVE_NETWORKINFO:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_SAVE_NETWORKINFO;

		uint8_t ip = (uint8_t)atoi(strarr[3]);

		SaveNetworkInfo((uint8_t*)strarr[1], (uint8_t*)strarr[2], ip);

		esp8266_ready_send_TCP(id, sizeof(cmd));
		g_Var.Wifi_State = 0;
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	case COMM_CMD_LOAD_NETWORKINFO:
	{
		g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_LOAD_NETWORKINFO;

		LoadNetworkInfo();

		esp8266_ready_send_TCP(id, sizeof(stNETWORK));
		g_Var.Cnt_Auto_Off = 0;
	}
		break;

	default:
		break;
	}

}

void ProcessComSend_WIFI(void)
{
	switch(g_esp8266.data_send_status)
	{
	case ESP8266_DATA_SEND_STATUS_NONE:
	{

	}
		break;

	case ESP8266_DATA_SEND_STATUS_RESET:
	{

	}
		break;

	case ESP8266_DATA_SEND_STATUS_SAVE_VERSION:
	{
		esp8266_send_data((uint8_t*)&(stVERSIONINFO.Cmd), sizeof(stVERSIONINFO.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_LOAD_VERSION:
	{
		esp8266_send_data((uint8_t*)&stVERSIONINFO, sizeof(stVERSIONINFO));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_STATUS:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_WAVE:
	{
		esp8266_send_data((uint8_t*)&(stWAVE.Cmd), sizeof(stWAVE.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_OFFSET_A:
	{
		esp8266_send_data((uint8_t*)&(stOFFSET_A.Cmd), sizeof(stOFFSET_A.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_OFFSET_B:
	{
		esp8266_send_data((uint8_t*)&(stOFFSET_B.Cmd), sizeof(stOFFSET_B.Cmd));
	}
		break;


	case ESP8266_DATA_SEND_STATUS_DOWNLINK_UPLOAD_ALARM:
	{
		esp8266_send_data((uint8_t*)&(stALARM.Cmd), sizeof(stALARM.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_WAVE:
	{
		esp8266_send_data((uint8_t*)&stWAVE, sizeof(stWAVE));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_OFFSET_A:
	{
		esp8266_send_data((uint8_t*)&stOFFSET_A, sizeof(stOFFSET_A));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_OFFSET_B:
	{
		esp8266_send_data((uint8_t*)&stOFFSET_B, sizeof(stOFFSET_B));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_LOAD_ALARM:
	{
		esp8266_send_data((uint8_t*)&stALARM, sizeof(stALARM));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_SET_ALARM_LIMIT:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_DOWNLINK_SET_BOUNDARY_OFFSET:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_WAVE:
	{
		esp8266_send_data((uint8_t*)&(stWAVE.Cmd), sizeof(stWAVE.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_OFFSET_A:
	{
		esp8266_send_data((uint8_t*)&(stOFFSET_A.Cmd), sizeof(stOFFSET_A.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_OFFSET_B:
	{
		esp8266_send_data((uint8_t*)&(stOFFSET_B.Cmd), sizeof(stOFFSET_B.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_UPLOAD_ALARM:
	{
		esp8266_send_data((uint8_t*)&(stALARM.Cmd), sizeof(stALARM.Cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_WAVE:
	{
		esp8266_send_data((uint8_t*)&stWAVE, sizeof(stWAVE));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_OFFSET_A:
	{
		esp8266_send_data((uint8_t*)&stOFFSET_A, sizeof(stOFFSET_A));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_OFFSET_B:
	{
		esp8266_send_data((uint8_t*)&stOFFSET_B, sizeof(stOFFSET_B));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_LOAD_ALARM:
	{
		esp8266_send_data((uint8_t*)&stALARM, sizeof(stALARM));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_UPLINK_SET_ALARM_LIMIT:
	{
		SendUIStatus_WIFI();
	}
		break;


	case ESP8266_DATA_SEND_STATUS_UPLINK_SET_BOUNDARY_OFFSET:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_LOAD_DATA:
	{
		esp8266_send_data((uint8_t*)&stDATA, sizeof(STC_DATA_BUF));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_OPTSW_DOWN:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_OPTSW_UP:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_SET_CHANNEL:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_SET_MAXDATALEN:
	{
		esp8266_send_data((uint8_t*)&g_esp8266.data_send_status, sizeof(g_esp8266.data_send_status));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_SET_ATT:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_RESET_ATT:
	{
		SendUIStatus_WIFI();
	}
		break;

	case ESP8266_DATA_SEND_STATUS_SAVE:
	{

	}
		break;

	case ESP8266_DATA_SEND_STATUS_SAVE_NETWORKINFO:
	{
		uint16_t cmd = COMM_CMD_SAVE_NETWORKINFO;
		esp8266_send_data((uint8_t*)&cmd, sizeof(cmd));
	}
		break;

	case ESP8266_DATA_SEND_STATUS_LOAD_NETWORKINFO:
	{
		esp8266_send_data((uint8_t*)&stNETWORK, sizeof(stNETWORK));
	}
		break;

	default:
		break;
	}

	g_esp8266.data_send_status = ESP8266_DATA_SEND_STATUS_NONE;
}

void SendUIStatus_WIFI(void)
{
	memcpy(stUI_STATUS.Optpower, stDT_REALTIME.OptPower, sizeof(stUI_STATUS.Optpower));
	stUI_STATUS.Cmd = COMM_CMD_STATUS;
	stUI_STATUS.Attenuation = stDT_SAVE[0].Attenuation;
	stUI_STATUS.Ref_Wavelen = stDT_SAVE[0].Ref_Wavelen;
	stUI_STATUS.Boundary_Offset[0] = stDT_SAVE[0].Boundary_Offset;
	stUI_STATUS.Boundary_Offset[1] = stDT_SAVE[1].Boundary_Offset;
	stUI_STATUS.OptPower_Alarm_Limit[0] = stDT_SAVE[0].Alarm_Temperallimit;
	stUI_STATUS.OptPower_Alarm_Limit[1] = stDT_SAVE[1].Alarm_Temperallimit;
	stUI_STATUS.CurrSelChannel = g_Var.CurrSelChannel;
	stUI_STATUS.Channellen = g_Var.EEP_stDT_Length;
	stUI_STATUS.CurrOptSW = g_Var.CurrOptSw;
	stUI_STATUS.Bat_Capacity = stSTATE.Bat_Capacity;
	stUI_STATUS.IN_Power = stSTATE.IN_Power;
	stUI_STATUS.Wifi_State = g_Var.Wifi_State;
	stUI_STATUS.Alarm.Bits.Proc = g_Var.Alarm_State.Bits.Proc;
	stUI_STATUS.Alarm.Bits.Func = g_Var.Alarm_State.Bits.Func;
	stUI_STATUS.Alarm.Bits.Boost = g_Var.Alarm_State.Bits.Boost;
	stUI_STATUS.Alarm.Bits.VBus = g_Var.Alarm_State.Bits.VBus;
	stUI_STATUS.Alarm.Bits.Bat = g_Var.Alarm_State.Bits.Bat;

	esp8266_send_data((uint8_t*)&stUI_STATUS, sizeof(stUI_STATUS));
}
