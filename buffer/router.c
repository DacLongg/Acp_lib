

#include "router.h"
#include "stdlib.h"



Router *newRouter(uint16_t packetSize, uint16_t frameSize, uint8_t packet_fragment, uint16_t cycleTime)
{
	Router* rout = (Router *)malloc(sizeof(Router));

	if (rout == NULL)
	{
		return rout;
	}

	rout->packetSize = packetSize;
	rout->frameSize = frameSize;
	rout->cycleTime = cycleTime;
	rout->packet_fragment = packet_fragment;

	rout->echoRx = false;
	rout->echoTx = false;
	rout->slipEnable = false;
	rout->noParody = false;

	rout->rxRing = (Ring *)newRing(packetSize * packet_fragment);
	rout->rxPacket = (Fifo *)newFifo(packetSize);
	rout->txPacket = (Fifo *)newFifo(packetSize);
	rout->parodyBuff = (Buffer *)newBuffer();
	return rout;
}

void Router_Dispose(Router *router)
{
	Ring_Dispose(router->rxRing);
	Fifo_Dispose(router->rxPacket);
	Fifo_Dispose(router->txPacket);
	Buffer_Dispose(router->parodyBuff);
	free(router);
}

void Router_SlipConfig(Router* router, bool Enable)
{
	router->slipEnable = Enable;
}

void Router_noParodyConfig(Router* router, bool Enable)
{
	Buffer_MergeAllData(router->parodyBuff);
	Buffer_DeleteData(router->parodyBuff, 0);
	router->noParody = Enable;
}

void Router_echoTxConfig(Router* router, bool Enable)
{
	router->echoTx = Enable;
}

void Router_echoRxConfig(Router* router, bool Enable)
{
	router->echoRx = Enable;
}

void Router_PushMessageTx(Router* router, uint8_t* data, uint16_t length)
{
	Buffer* dataEnc = NULL;

	if (router->slipEnable == true)
	{
        if(length > router->packet_fragment)
        {
            uint8_t NumbPack = length / router->packet_fragment;
            if((NumbPack * router->packet_fragment) < length)
            {
            	NumbPack += 1;
            }
            dataEnc = Buffer_SlipSplitPack(SLIP_HEADFRAME, data, router->packet_fragment);
            if(dataEnc != NULL)
            {
    			Fifo_AddData(router->txPacket, dataEnc->data, dataEnc->length);

    			if (router->echoTx == true)
    			{
    				Ring_PushData(router->rxRing, dataEnc->data, dataEnc->length);
    			}
    			Buffer_Dispose(dataEnc);
            }
            for(uint8_t countPack = 1; countPack < NumbPack; countPack ++)
            {
                uint16_t NumBdata =  (countPack) * router->packet_fragment;
                if((countPack + 1) * router->packet_fragment >= length)
                {
                	break;
                }
                dataEnc = Buffer_SlipSplitPack(SLIP_MIDDLEFRAME, &data[NumBdata], router->packet_fragment);
                if(dataEnc != NULL)
                {
        			Fifo_AddData(router->txPacket, dataEnc->data, dataEnc->length);

        			if (router->echoTx == true)
        			{
        				Ring_PushData(router->rxRing, dataEnc->data, dataEnc->length);
        			}
        			Buffer_Dispose(dataEnc);
                }
            }
            dataEnc = Buffer_SlipSplitPack(SLIP_ENDFRAME, &data[(NumbPack - 1) * router->packet_fragment], (length - ((NumbPack - 1) * router->packet_fragment)));
            if(dataEnc != NULL)
            {
    			Fifo_AddData(router->txPacket, dataEnc->data, dataEnc->length);

    			if (router->echoTx == true)
    			{
    				Ring_PushData(router->rxRing, dataEnc->data, dataEnc->length);
    			}
    			Buffer_Dispose(dataEnc);
            }

        }
        else
        {
    		dataEnc = Buffer_SlipEnc(data, length);
    		if (dataEnc != NULL)
    		{
    			Fifo_AddData(router->txPacket, dataEnc->data, dataEnc->length);

    			if (router->echoTx == true)
    			{
    				Ring_PushData(router->rxRing, dataEnc->data, dataEnc->length);
    			}
    			Buffer_Dispose(dataEnc);
    		}
        }
//		dataEnc = Buffer_SlipEnc(data, length);
//		if (dataEnc != NULL)
//		{
//			Fifo_AddData(router->txPacket, dataEnc->data, dataEnc->length);    //  "version default"
//
//			if (router->echoTx == true)
//			{
//				Ring_PushData(router->rxRing, dataEnc->data, dataEnc->length);
//			}
//			Buffer_Dispose(dataEnc);
//		}
	}
	else
	{
		Fifo_AddData(router->txPacket, data, length);
		if (router->echoTx == true)
		{
			Ring_PushData(router->rxRing, data, length);
		}
	}

	if (router->noParody == true)
	{
		Buffer_AddData(router->parodyBuff, data, length);
	}

}

Router_ReturnType Router_PushMessageRx(Router* router, uint8_t* data, uint16_t length)
{
	return Ring_PushData(router->rxRing, data, length);
}

Router_ReturnType Router_PullMessageTx(Router* router, uint8_t* data, uint16_t* length)
{
	return Fifo_GetData(router->txPacket, data, length);
}

uint16_t Router_CountDataTx(Router* router)
{
	return Fifo_GetSizeData(router->txPacket);
}

uint16_t Router_CountDataRx(Router * router)
{
	return Fifo_GetSizeData(router->rxPacket);
}

Router_ReturnType Router_PullMessageRx(Router* router, uint8_t* data, uint16_t* length)
{
	uint16_t lenCount = Fifo_GetSizeData(router->rxPacket);

	if (lenCount != 0)
	{
		uint16_t posData = 0;

		if (router->echoRx == true)
		{
			Fifo_AddData(router->rxPacket, data, *length);
		}

		return Fifo_GetData(router->rxPacket, data, length);
	}
	else
	{
		return BUFF_NOT_OK;
	}
}

__attribute__((optimize("Ofast"))) Router_ReturnType Router_MessageHandler(Router* router)
{
	Buffer* dataDec = NULL;
	static uint16_t countCycle = 0; 
	static Buffer* dataBuf = NULL;

	if(dataBuf == NULL)
	{
		dataBuf = newBuffer();
	}

	if (router->slipEnable == true)
	{
		uint16_t pos = 0;
		if (Ring_FindByte(router->rxRing, 0xC0, &pos) > 0)
		{
			uint16_t len = Ring_CountBytePos(router->rxRing, pos);
			uint8_t* dataraw = (uint8_t*)malloc(len);
			if (dataraw == NULL)
			{
				return BUFF_NOT_OK;
			}

//			if (len == 1)    								// change
//			{
//				Ring_DeleteData(router->rxRing, pos);
//			}

			if (len > router->frameSize || len == 1)    //default
			{
				Ring_DeleteData(router->rxRing, pos);
			}
			else
			{
				if (BUFF_OK == Ring_GetData(router->rxRing, dataraw, &len, pos))
				{
					dataDec = Buffer_SlipDec(dataraw, len);
					if (dataDec != NULL)
					{
						if (router->noParody == true)
						{
							if (BUFF_OK == Buffer_SearchData(router->parodyBuff, dataDec->data, dataDec->length, &pos))
							{
								Buffer_DeleteData(router->parodyBuff, pos);
								Buffer_Dispose(dataDec);
								free(dataraw);
								return BUFF_NOT_OK;
							}
							else if (Buffer_CountData(router->parodyBuff) >= 10)
							{
								Buffer_DeleteData(router->parodyBuff, 0);
							}
						}
						Fifo_AddData(router->rxPacket, dataDec->data, dataDec->length);
						Buffer_Dispose(dataDec);
					}
//					free(dataraw);
				}
			}
			free(dataraw);
			return BUFF_OK;
		}
		else if (Fifo_CountData(router->txPacket) > 0)
		{
			return BUFF_OK;
		}
		else
		{
			return BUFF_NOT_OK;
		}
	}
	else
	{
		uint16_t pos = 0;
		uint16_t len = Ring_CountByte(router->rxRing);

//		uint8_t* dataraw = (uint8_t*)malloc(len*sizeof(uint8_t));
//		if (dataraw == NULL)
//		{
//			return BUFF_NOT_OK;
//		}

		if (len > 0)
		{
			uint8_t* dataraw = (uint8_t*)malloc(len*sizeof(uint8_t));
			if (dataraw == NULL)
			{
				return BUFF_NOT_OK;
			}
			Ring_PullData(router->rxRing, dataraw, len);
			Buffer_AppendData(dataBuf, 0, dataraw, len);
			free(dataraw);
			dataraw = NULL;
			countCycle = 0;
		}
		else
		{
			if (++countCycle >= router->cycleTime || router->cycleTime == 0)
			{
				if (dataBuf->length != 0 && dataBuf->data != NULL)
				{
					if (router->noParody == true)
					{
						if (BUFF_OK == Buffer_SearchData(router->parodyBuff, dataBuf->data, dataBuf->length, &pos))
						{
							Buffer_DeleteData(router->parodyBuff, pos);
							Buffer_DeleteData(dataBuf, 0);
							return BUFF_NOT_OK;
						}
					}

					Fifo_AddData(router->rxPacket, dataBuf->data, dataBuf->length);
					dataBuf->length = 0;
					Buffer_DeleteData(dataBuf, 0);
				}
				countCycle = 0;
			}
		}
		return BUFF_OK;
	}
 
}


