#ifndef __ROUTER_H
#define __ROUTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
//#include "crc.h"
#include "buffer.h"
#include "stdbool.h"


typedef uint8_t Router_ReturnType;

typedef struct
{
    Ring *rxRing;
    Buffer* parodyBuff;

    Fifo *rxPacket;
    Fifo *txPacket;

    uint16_t packetSize;
    uint16_t frameSize;
    uint16_t cycleTime;
    uint8_t  packet_fragment;

    bool slipEnable;
    bool noParody;
    bool echoRx;
    bool echoTx;
}Router;

//ACPLIB_API
Router *newRouter(uint16_t packetSize, uint16_t frameSize, uint8_t packet_fragment, uint16_t cycleTime);

//ACPLIB_API
void Router_Dispose(Router* router);

//ACPLIB_API
void Router_SlipConfig(Router* router, bool Enable);

//ACPLIB_API
void Router_noParodyConfig(Router* router, bool Enable);

//ACPLIB_API
void Router_echoTxConfig(Router* router, bool Enable);

//ACPLIB_API
void Router_echoRxConfig(Router* router, bool Enable);

//ACPLIB_API
uint16_t Router_CountDataTx(Router* router);

//ACPLIB_API
uint16_t Router_CountDataRx(Router* router);

//ACPLIB_API
__attribute__((optimize("Ofast"))) Router_ReturnType Router_MessageHandler(Router* router);

//ACPLIB_API
void Router_PushMessageTx(Router* router, uint8_t* data, uint16_t length);

//ACPLIB_API
Router_ReturnType Router_PushMessageRx(Router* router, uint8_t* data, uint16_t length);

//ACPLIB_API
Router_ReturnType Router_PullMessageTx(Router* router, uint8_t* data, uint16_t* length);

//ACPLIB_API
Router_ReturnType Router_PullMessageRx(Router* router, uint8_t* data, uint16_t* length);

#ifdef __cplusplus
}
#endif

#endif
