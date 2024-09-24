#ifndef __ACP_H
#define __ACP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "main.h"
#include "crc.h"
#include "router.h"

#define ACP_FIN		0x01
#define ACP_SYN		0x02
#define ACP_RST		0x04
#define ACP_PSH		0x08
#define ACP_ACK		0x10
#define ACP_URG		0x20

#define  ACP_NULL            (void *)0
#define  ACP_OK              0x00
#define  ACP_NOT_OK          0x01

typedef uint8_t Acp_ReturnType;


typedef union
{
	struct
	{
		uint8_t Encode;
		uint8_t Version;
		uint8_t IP_length;
		uint8_t Option;
		uint16_t TotalLength;
		uint16_t IP_sourceAddr;
		uint16_t IP_destAddr;
		uint8_t TimeOflive;
		uint8_t Crc;
	}param;
	uint8_t rawdata[12];
}IP_head_t;

typedef union
{
	struct{
		uint8_t FIN :1;
		uint8_t SYN :1;
		uint8_t RST :1;
		uint8_t PSH :1;
		uint8_t ACK :1;
		uint8_t URG :1;
		uint8_t rev :2;
	}bit;
	uint8_t byte;
}acp_flag_t;

typedef union
{
	struct{
		uint8_t 	SourcePort;
		uint8_t 	DestPort;
		uint8_t 	SeqNumber;
		uint8_t 	AckNumber;
		acp_flag_t 	Flag;
		uint8_t 	Crc;
	}param;
	uint8_t rawdata[6];
}TCP_head_t;

typedef struct
{
	IP_head_t  	IP_Head;
	uint8_t 	*IP_padding;
	TCP_head_t 	TCP_Head;
	uint8_t 	*TCP_padding;
}acp_frame_t;

typedef enum
{
	ACP_CLOSE,
	ACP_OPEN,
	ACP_RCVD,
	ACP_SEND,
	ACP_ESTABLISHED,
	ACP_FIN_WAIT_1,
	ACP_CLOSE_WAIT,
	ACP_LAST_ACK,
	ACP_FIN_WAIT_2,
	ACP_TIME_WAIT,
}acp_conn_state;

typedef struct
{
	uint16_t  addr;
	uint8_t   port;
}acp_point;


typedef struct
{
	acp_flag_t Flag;
	uint8_t    TimeToLive;
	acp_point  src;
	acp_point  des;
	uint8_t    SeqNumber;
    uint8_t    AckNumber;
	acp_conn_state state;
	Fifo     *rxBuff;
	Fifo     *txBuff;
}acp_conn;



typedef void (* acp_notify_connection)(acp_conn *conn, uint8_t port);
typedef void (* acp_notify_receiver)(acp_conn *conn);

typedef struct
{
	uint8_t maxConn;
	uint8_t maxPort;
	uint16_t ip_addr;
	acp_conn *ListConn;
	uint8_t  *listPort;
	Fifo	 *fifo;
	Fifo	 *fifo_send;
	acp_notify_connection 	Acp_connected;
	acp_notify_connection   Acp_disconnected;
}acp;


acp *newAcp(uint16_t ip_addr, uint8_t maxConn, uint8_t maxPort);
void Acp_SetupIpaddr(acp *acp_obj, uint16_t ip_addr);
Acp_ReturnType Acp_Listen(acp *acp_obj, uint8_t port);
void Acp_Dispose(acp *acp_obj);
__attribute__((optimize("Ofast"))) void Acp_Process(acp *acp_obj, uint16_t Timecycle);
void Acp_pushMessConn(acp_conn *acp_conn, uint8_t *data, uint16_t length);
Acp_ReturnType Acp_getMessConn(acp_conn *acp_conn, uint8_t *data, uint16_t *length);
uint16_t Acp_countDataConn(acp_conn *acp_conn);
void Acp_pushMessage(acp *acp_obj, uint8_t *data, uint16_t length);
Acp_ReturnType Acp_pullMessage(acp *acp_obj, uint8_t *data, uint16_t *length);
acp_conn * Acp_Connect(acp *acp_obj, acp_point des);
void Acp_close(acp_conn *connection);
Acp_ReturnType Acp_getConn(acp *acp_obj, uint8_t port, acp_conn *conn);
void Acp_EventClosed(acp *acp_obj, acp_notify_connection notify);
void Acp_EventEstablish(acp *acp_obj, acp_notify_connection notify);
Acp_ReturnType Acp_FindConn(acp *acp_obj, acp_point Scr, acp_point Des, uint8_t *pos);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
