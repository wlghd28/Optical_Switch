/*
 * esp8266.c
 *
 *  Created on: Aug 23, 2023
 *      Author: hojin
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp8266.h"
#include "usart.h"
#include "sw.h"
#include "hw.h"

#define hEsp8266 huart2

ESP8266_GLOBAL_VAR g_esp8266;
ESP8266_NETWORK_BUF esp8266_NETWORK;

void esp8266_send_cmd(const char *strcmd)
{
	uint16_t cmdlen =  strlen(strcmd);
	HAL_UART_Transmit(&hEsp8266, (uint8_t*)strcmd, cmdlen, 10);
	HAL_UART_Transmit(&hEsp8266, (uint8_t *)"\x0D\x0A", 2, 10);
}

void esp8266_send_data(uint8_t *data, size_t size)
{
	//HAL_Delay(ESP8266_INTERVAL_COMM);
	for(int i = 0; i < size; i++)
	{
		HAL_UART_Transmit(&hEsp8266, data + i, 1, 10);
	}
}

void esp8266_start(uint8_t val)
{
	if(val)
	{
		SetWifiONOFF(1);
		HAL_Delay(100);
		SetWifiEN(1);
		HAL_Delay(100);
		SetWifiRST(1);
		HAL_Delay(1000);	// Wifi 전원 딜레이 줘야함..
	}
	else
	{
		SetWifiONOFF(0);
	}
}

void esp8266_init(void)
{

//	// Test
//	esp8266_send_cmd("AT");
//	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);
//
//	esp8266_send_cmd("AT+GMR");
//	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);

	// ECHO off
	esp8266_send_cmd("ATE0");	// ATE0 : OFF
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);

	// DHCP Server Setting
	esp8266_send_cmd("AT+CWDHCP=0,1");	// Mode : SoftAP, DHCP : Enable
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);
}

void esp8266_startserver_TCP(void)
{
	char tmpcmd[ESP8266_STRCMD_LEN] = { 0, };

	// Set Host name
//	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
//	sprintf(tmpcmd, "AT+CWHOSTNAME=%s", ESP8266_HOSTNAME);
//	esp8266_send_cmd((const char*)tmpcmd);

	// Set Server IP
	if (g_Var.EEP_Add_Init != INIT_VAL_EEP)
	{
		memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
		sprintf(tmpcmd, "AT+CIPAP=%s,%s,%s", ESP8266_DEFAULTIP, ESP8266_GATEWAY, ESP8266_SUBNETMASK);
		esp8266_send_cmd((const char*)tmpcmd);
		HAL_Delay(ESP8266_WAIT_RESPOND_TIME);
	}

	// Start TCP Server
	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
	sprintf(tmpcmd, "AT+CWMODE=%d", ESP8266_CWMODE);
	esp8266_send_cmd((const char*)tmpcmd);
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);

	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
	sprintf(tmpcmd, "AT+CIPMUX=%d", ESP8266_CIPMUX);
	esp8266_send_cmd((const char*)tmpcmd);
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);

	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
	sprintf(tmpcmd, "AT+CIPSERVER=1,%d", ESP8266_PORT);
	esp8266_send_cmd((const char*)tmpcmd);
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);
}

void esp8266_ready_send_TCP(uint8_t id, uint16_t size)
{
	char tmpcmd[30] = { 0, };
	sprintf(tmpcmd, "AT+CIPSEND=%d,%d", id, size);
	esp8266_send_cmd((const char*)tmpcmd);
}

void esp8266_set_networkinfo(uint8_t* wifi_id, uint8_t* wifi_pwd, uint8_t ip)
{
	char tmpcmd[ESP8266_STRCMD_LEN] = { 0, };

	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
	sprintf(tmpcmd, "AT+CIPAP=\"192.168.4.%03d\",%s,%s", ip, ESP8266_GATEWAY, ESP8266_SUBNETMASK);
	esp8266_send_cmd((const char*)tmpcmd);
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);

	memset(tmpcmd, 0, ESP8266_STRCMD_LEN * sizeof(char));
	sprintf(tmpcmd, "AT+CWSAP=\"%s\",\"%s\",1,3,1,0", wifi_id, wifi_pwd);
	esp8266_send_cmd((const char*)tmpcmd);
	HAL_Delay(ESP8266_WAIT_RESPOND_TIME);
}

