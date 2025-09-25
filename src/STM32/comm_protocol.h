/*
 * comm_protocol.h
 *
 *  Created on: Aug 29, 2023
 *      Author: hojin
 */

#ifndef INC_COMM_PROTOCOL_H_
#define INC_COMM_PROTOCOL_H_

#define WIFI_ID_LEN                 32
#define WIFI_PWD_LEN                12

// Communicate command
// UART Command
//{
//    USB,CMD,~~\n
//}

// TCP Command
//{
//    CMD,~~\n
//}

enum
{
    COMM_CMD_RESET = 0x803B, // 1바이트 값이 '\r',',',':'되는 값을 피해준다.
	COMM_CMD_SAVE_VERSION,
    COMM_CMD_LOAD_VERSION,
    COMM_CMD_STATUS,
    COMM_CMD_DOWNLINK_UPLOAD_WAVE,
    COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A,
    COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B,
	COMM_CMD_DOWNLINK_UPLOAD_ALARM,
    COMM_CMD_DOWNLINK_LOAD_WAVE,
    COMM_CMD_DOWNLINK_LOAD_OFFSET_A,
    COMM_CMD_DOWNLINK_LOAD_OFFSET_B,
	COMM_CMD_DOWNLINK_LOAD_ALARM,
	COMM_CMD_DOWNLINK_SET_ALARM_LIMIT,
	COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET,
	COMM_CMD_UPLINK_UPLOAD_WAVE,
	COMM_CMD_UPLINK_UPLOAD_OFFSET_A,
	COMM_CMD_UPLINK_UPLOAD_OFFSET_B,
	COMM_CMD_UPLINK_UPLOAD_ALARM,
	COMM_CMD_UPLINK_LOAD_WAVE,
	COMM_CMD_UPLINK_LOAD_OFFSET_A,
	COMM_CMD_UPLINK_LOAD_OFFSET_B,
	COMM_CMD_UPLINK_LOAD_ALARM,
	COMM_CMD_UPLINK_SET_ALARM_LIMIT,
	COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET,
    COMM_CMD_LOAD_DATA,
    COMM_CMD_OPTSW_DOWN,
    COMM_CMD_OPTSW_UP,
    COMM_CMD_SET_CHANNEL,
	COMM_CMD_SET_MAXDATALEN,
    COMM_CMD_SET_ATT,
	COMM_CMD_RESET_ATT,
    COMM_CMD_SAVE,
	COMM_CMD_SAVE_NETWORKINFO,
	COMM_CMD_LOAD_NETWORKINFO,
    COMM_CMD_LEN
};

#pragma pack(1)
typedef struct
{
    uint16_t Cmd;
    uint8_t Index;
    uint32_t Wavelen;
}STC_WAVE_BUF;

typedef struct
{
    uint16_t Cmd;
    uint8_t Index;
    int16_t Offset;
}STC_OFFSET_BUF;

typedef struct
{
	uint16_t Cmd;
	uint8_t Index;
    uint16_t Limit;
	uint8_t Enable;
}STC_ALARM_BUF;

typedef struct
{
    uint16_t Cmd;
    uint8_t Index;
    uint32_t Wavelen;
    int16_t OptPower;
    int16_t Attenuation;
    uint8_t OptSw;
}STC_DATA_BUF;


typedef struct
{
	uint16_t Cmd;
	uint8_t ModuleID[8];
}STC_VERSIONINFO_BUF;

typedef struct
{
    uint16_t Cmd;
    uint8_t ID[WIFI_ID_LEN];
    uint8_t Pwd[WIFI_PWD_LEN];
    uint8_t IP[4];
}STC_NETWORK_BUF;

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
		uint8_t B6 : 1;
		uint8_t B7 : 1;
    } Bits;
}UNI_BYTE_UI_ALARM;

typedef struct
{
	uint16_t Cmd;
    int16_t Optpower[ADC_DT_MAXLEN];
    int16_t Attenuation;
    uint32_t Ref_Wavelen;
    int32_t Boundary_Offset[2]; // 0 : Down, 1 : Up
    int16_t OptPower_Alarm_Limit[2]; // 0 : Down, 1 : UP
    uint8_t CurrSelChannel; // 0 ~ 17
    uint8_t Channellen; // Max : 18
    uint8_t CurrOptSW;  // 0 : Down, 1 : Up
    uint16_t Bat_Capacity;
    uint16_t IN_Power;
    uint8_t Wifi_State;	// 0 : Off, 1 : On
    UNI_BYTE_UI_ALARM Alarm;
}STC_UI_STATUS_BUF;
#pragma pack()


#endif /* INC_COMM_PROTOCOL_H_ */
