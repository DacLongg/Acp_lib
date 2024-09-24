#ifndef BUFFER_H
#define BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdint.h"
//typedef   signed long  int32_t;
//typedef unsigned long uint32_t;

typedef uint8_t Buff_ReturnType;

#define  BUFF_NULL           NULL
#define  BUFF_OK             0x00
#define  BUFF_NOT_OK         0x01

#define  SLIP_HEADFRAME      0x01
#define  SLIP_MIDDLEFRAME    0x02
#define  SLIP_ENDFRAME       0x03
#define  SLIP_FULLFRAME      0x04   

typedef struct Buffer
{
    uint8_t* data;
    uint16_t length;
    struct Buffer* link;
} Buffer;


typedef struct
{
    uint16_t size;
    int16_t head;
    int16_t tail;
    uint8_t* data;
} Ring;

typedef struct
{
    void* addr;
    uint16_t length;
}Object;

typedef struct ListObj
{
    Object* obj;
    struct ListObj* next;
}ListObj;

typedef struct
{
    uint16_t size;
    ListObj* objs;
} Fifo;


//ACPLIB_API
Buffer* newBuffer(void);

//ACPLIB_API
Buff_ReturnType Buffer_AddData(Buffer* buff, uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
uint16_t Buffer_GetSizeData(Buffer* buff, uint16_t position);

//ACPLIB_API
uint8_t *Buffer_GetPtrData(Buffer* buff, uint16_t position);

//ACPLIB_API
Buff_ReturnType Buffer_GetData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t* length);

//ACPLIB_API
Buff_ReturnType Buffer_AppendData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
Buff_ReturnType Buffer_ReplaceData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
Buff_ReturnType Buffer_SearchData(Buffer* buff, uint8_t* u8DataPtr, uint16_t length, uint16_t* position);

//ACPLIB_API
Buff_ReturnType Buffer_SplitData(Buffer* buff, uint16_t posBuff, uint16_t posData);

//ACPLIB_API
Buff_ReturnType Buffer_MergeData(Buffer* buff, uint16_t posSrc, uint16_t posDes);

//ACPLIB_API
Buff_ReturnType Buffer_MergeAllData(Buffer* buff);

//ACPLIB_API
Buff_ReturnType Buffer_DeleteData(Buffer* buff, uint16_t position);

//ACPLIB_API
uint16_t Buffer_CountData(Buffer* buff);

//ACPLIB_API
Buffer* Buffer_SlipSplitPack(uint8_t TypeFrame, uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
Buffer* Buffer_SlipEnc(uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
Buffer* Buffer_SlipDec(uint8_t* u8DataPtr, uint16_t length);

//ACPLIB_API
void Buffer_Dispose(Buffer* buff);

//ACPLIB_API
Ring* newRing(uint16_t size);

//ACPLIB_API
uint16_t Ring_CountByte(Ring* _ring);

//ACPLIB_API
uint16_t Ring_CountBytePos(Ring* _ring, uint16_t Pos);

//ACPLIB_API
Buff_ReturnType Ring_PushData(Ring* _ring, uint8_t* data, uint16_t length);

//ACPLIB_API
int16_t Ring_PullData(Ring* _ring, uint8_t* data, uint16_t length);

//ACPLIB_API
Buff_ReturnType Ring_GetData(Ring* _ring, uint8_t* data, uint16_t* length, uint16_t pos);

//ACPLIB_API
uint16_t Ring_FindByte(Ring* _ring, uint8_t Byte, uint16_t* pos);

//ACPLIB_API
Buff_ReturnType Ring_DeleteData(Ring* _ring, uint16_t Pos);

//ACPLIB_API
void Ring_Dispose(Ring* ring);

//ACPLIB_API
Fifo *newFifo(uint16_t size);

//ACPLIB_API
uint16_t Fifo_GetSizeData(Fifo* fifo);

//ACPLIB_API
Buff_ReturnType Fifo_AddData(Fifo* fifo, void* data, uint16_t length);

//ACPLIB_API
Buff_ReturnType Fifo_GetData(Fifo* fifo, void* data, uint16_t* length);

//ACPLIB_API
uint16_t Fifo_CountData(Fifo *fifo);

//ACPLIB_API
void Fifo_Dispose(Fifo* fifo);



#ifdef __cplusplus
}
#endif

#endif
