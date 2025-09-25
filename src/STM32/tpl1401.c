/*
 * tpl1401.c
 *
 *  Created on: Feb 6, 2024
 *      Author: User
 */

#include "tpl1401.h"
#include "sw.h"

UNI_TPL1401_STATUS TPL1401_Status;
UNI_TPL1401_GENERAL_CONFIG TPL1401_General_Config;
UNI_TPL1401_PROTECT TPL1401_Protect;
UNI_TPL1401_DPOT_POSITION TPL1401_Dpot_Position;

//UNI_TPL1401_STATUS TPL1401_Status_Test;
//UNI_TPL1401_GENERAL_CONFIG TPL1401_General_Config_Test;
//UNI_TPL1401_PROTECT TPL1401_Protect_Test;
//UNI_TPL1401_DPOT_POSITION TPL1401_Dpot_Position_Test;

void tpl1401_init(void)
{
//	SetTPL1401_SCL(1);
//	HAL_Delay(10);
//	SetTPL1401_SDA(1);
	HAL_Delay(100);

	TPL1401_General_Config.Bits.Out_Span = 0;
	TPL1401_General_Config.Bits.Ref_En = 0;
	TPL1401_General_Config.Bits.Dpot_Pdn = 0x00;	// 00 : Power Up
	TPL1401_General_Config.Bits.Rsv0 = 0x0F;
	TPL1401_General_Config.Bits.Dev_Lock = 0;
	TPL1401_General_Config.Bits.Rsv1 = 0;

	TPL1401_General_Config.Val = __REV16(TPL1401_General_Config.Val);	// 바이트를 뒤집어야 통신이 됨.

	WriteTPL1401(SLAVE_REG_ADD_TPL1401_GENERAL_CONFIG, (uint8_t*)(TPL1401_General_Config.Byte), sizeof(TPL1401_General_Config));

//	TPL1401_Protect.Bits.SW_Reset = 0x08;
//	TPL1401_Protect.Bits.Dev_Unlock_Code = 0x05;
//
//	WriteTPL1401(SLAVE_REG_ADD_TPL1401_PROTECT, (uint8_t*)(&TPL1401_Protect), sizeof(TPL1401_Protect));
}

void tpl1401_Set_Dpot_Pos(uint8_t val)
{
	TPL1401_Dpot_Position.Bits.Rsv0 = 0;
	TPL1401_Dpot_Position.Bits.Rsv1 = 0;
	TPL1401_Dpot_Position.Bits.Dpot_Pos = val;
	TPL1401_Dpot_Position.Bits.Rsv2 = 0;
	//TPL1401_Dpot_Position.Val = 0xFFFF;

	TPL1401_Dpot_Position.Val = __REV16(TPL1401_Dpot_Position.Val);

	WriteTPL1401(SLAVE_REG_ADD_TPL1401_DPOT_POSITION, (uint8_t*)(TPL1401_Dpot_Position.Byte), sizeof(TPL1401_Dpot_Position));
	WriteTPL1401(SLAVE_NVMREG_ADD_TPL1401_DPOT_POSITION, (uint8_t*)(TPL1401_Dpot_Position.Byte), sizeof(TPL1401_Dpot_Position));

//	ReadTPL1401(SLAVE_REG_ADD_TPL1401_STATUS, (uint8_t*)&TPL1401_Status_Test, sizeof(TPL1401_Status_Test));
//	ReadTPL1401(SLAVE_REG_ADD_TPL1401_GENERAL_CONFIG, (uint8_t*)&TPL1401_General_Config_Test, sizeof(TPL1401_General_Config_Test));
//	ReadTPL1401(SLAVE_REG_ADD_TPL1401_PROTECT, (uint8_t*)&TPL1401_Protect_Test, sizeof(TPL1401_Protect_Test));
//	ReadTPL1401(SLAVE_REG_ADD_TPL1401_DPOT_POSITION, (uint8_t*)&TPL1401_Dpot_Position_Test, sizeof(TPL1401_Dpot_Position_Test));

}
