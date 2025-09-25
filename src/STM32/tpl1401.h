/*
 * tpl1401.h
 *
 *  Created on: Feb 2, 2024
 *      Author: User
 */
#include <stdint.h>

#ifndef INC_TPL1401_H_
#define INC_TPL1401_H_

#define SLAVE_DEV_ADD_TPL1401_BROADCAST			0x8E
#define SLAVE_DEV_ADD_TPL1401_READ				0x91
#define SLAVE_DEV_ADD_TPL1401_WRITE				0x90
#define SLAVE_REG_ADD_TPL1401_STATUS			0xD0
#define SLAVE_REG_ADD_TPL1401_GENERAL_CONFIG	0xD1
#define SLAVE_REG_ADD_TPL1401_PROTECT			0xD3
#define SLAVE_REG_ADD_TPL1401_DPOT_POSITION		0x21
#define SLAVE_REG_ADD_TPL1401_USER_BYTE1		0x25
#define SLAVE_REG_ADD_TPL1401_USER_BYTE2		0x26
#define SLAVE_NVMREG_ADD_TPL1401_DPOT_POSITION	0x10


#pragma pack(1)
typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
    struct
	{
		uint16_t Ver_ID : 2;
		uint16_t Dev_ID : 4;
		uint16_t Rsv0 : 6;
		uint16_t Rsv1 : 1;
		uint16_t Nvm_Busy : 1;
		uint16_t Nvm_Crc_Alarm_Internal : 1;
		uint16_t Nvm_Crc_Alarm_User : 1;
	} Bits;
}UNI_TPL1401_STATUS;

typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
	struct
	{
		uint16_t Out_Span : 2;
		uint16_t Ref_En : 1;
		uint16_t Dpot_Pdn : 2;
		uint16_t Rsv0 : 8;
		uint16_t Dev_Lock : 1;
		uint16_t Rsv1 : 2;
	} Bits;
}UNI_TPL1401_GENERAL_CONFIG;

typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
    struct
    {
		uint16_t SW_Reset : 4;
		uint16_t Nvm_Prog : 1;
		uint16_t Nvm_Reload : 1;
		uint16_t Rsv0 : 3;
		uint16_t Dev_Config_Reset : 1;
		uint16_t Rsv1 : 2;
		uint16_t Dev_Unlock_Code : 4;
    } Bits;
}UNI_TPL1401_PROTECT;

typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
    struct
    {
		uint16_t Rsv0 : 2;
		uint16_t Rsv1 : 2;
		uint16_t Dpot_Pos : 8;
		uint16_t Rsv2 : 4;
    } Bits;
}UNI_TPL1401_DPOT_POSITION;

typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
    struct
	{
		uint16_t Rsv0 : 2;
		uint16_t User_Byte : 10;
		uint16_t Rsv1 : 4;
	} Bits;
}UNI_TPL1401_USER_BYTE1;

typedef union
{
	uint16_t Val;
	uint8_t Byte[2];
    struct
    {
		uint16_t Rsv0 : 2;
		uint16_t User_Byte : 10;
		uint16_t Rsv1 : 4;
    } Bits;
}UNI_TPL1401_USER_BYTE2;
#pragma pack()


extern void tpl1401_init(void);
extern void tpl1401_Set_Dpot_Pos(uint8_t val);

#endif /* INC_TPL1401_H_ */
