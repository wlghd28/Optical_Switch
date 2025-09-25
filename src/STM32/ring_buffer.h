/*
 * ring_buffer.h
 *
 *  Created on: Aug 11, 2023
 *      Author: hojin
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include <stdint.h>

#define RxBufSIZE					512		// Max Rx Buffer size

#pragma pack(1)
typedef struct
{
	uint16_t Head, Tail;
	uint8_t Buffer[RxBufSIZE];
}RING_BUF;
#pragma pack()

extern RING_BUF RingBuf_DEBUG;
extern RING_BUF RingBuf_USB;
extern RING_BUF RingBuf_WIFI;

extern void RingBuf_Insert(RING_BUF *RingBuf, uint8_t recvbuf);
extern void RingBuf_Delete(RING_BUF *RingBuf, uint16_t size);
extern uint8_t RingBuf_Getdata(RING_BUF *RingBuf, uint16_t index);
extern uint16_t RingBuf_Getdatalength(RING_BUF *RingBuf);
extern uint16_t RingBuf_Gethead(RING_BUF *RingBuf);
extern uint16_t RingBuf_Gettail(RING_BUF *RingBuf);


#endif /* INC_RING_BUFFER_H_ */
