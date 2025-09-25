/*
 * ring_buffer.c
 *
 *  Created on: Aug 11, 2023
 *      Author: hojin
 */


#include "ring_buffer.h"

RING_BUF RingBuf_DEBUG;
RING_BUF RingBuf_USB;
RING_BUF RingBuf_WIFI;

void RingBuf_Insert(RING_BUF *RingBuf, uint8_t recvbuf)
{

	RingBuf->Buffer[RingBuf->Head++] = recvbuf;
	RingBuf->Head &= (RxBufSIZE - 1);
	if(RingBuf->Head == RingBuf->Tail)
	{
		RingBuf->Tail++;
		RingBuf->Tail &= (RxBufSIZE - 1);
	}
}

void RingBuf_Delete(RING_BUF *RingBuf, uint16_t size)
{
	RingBuf->Tail += size;
	RingBuf->Tail &= (RxBufSIZE - 1);
}

uint8_t RingBuf_Getdata(RING_BUF *RingBuf, uint16_t index)
{
	uint16_t lindex = index;
	lindex &= (RxBufSIZE - 1);
	return RingBuf->Buffer[lindex];
}

uint16_t RingBuf_Getdatalength(RING_BUF *RingBuf)
{
	uint16_t ret = 0;
	if(RingBuf->Head >= RingBuf->Tail) ret = RingBuf->Head - RingBuf->Tail;
	else ret = RxBufSIZE - RingBuf->Tail + RingBuf->Head;
	return ret;
}

uint16_t RingBuf_Gethead(RING_BUF *RingBuf)
{
	return RingBuf->Head;
}

uint16_t RingBuf_Gettail(RING_BUF *RingBuf)
{
	return RingBuf->Tail;
}
