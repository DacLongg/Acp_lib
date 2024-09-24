// acplib.cpp : Defines the exported functions for the DLL.
//

#include "buffer.h"
#include "stdlib.h"

#define SLIP_END                0xC0
#define SLIP_ESC                0xDB
#define SLIP_ESC_END            0xDC
#define SLIP_ESC_ESC            0xDD

Buffer* newBuffer(void)
{
    Buffer* _newBuff = (Buffer*)malloc(sizeof(Buffer));
    if (_newBuff != BUFF_NULL)
    {
        _newBuff->data = BUFF_NULL;
        _newBuff->length = 0;
        _newBuff->link = BUFF_NULL;
    }
    return _newBuff;
}

Buff_ReturnType Buffer_AddData(Buffer* buff, uint8_t* u8DataPtr, uint16_t length)
{
    Buffer* _buff = buff;
    Buffer* head_buff = buff;
    
    while (_buff != BUFF_NULL)
    {
        if (_buff->length != 0 && _buff->data != BUFF_NULL)
        {
            head_buff = _buff;
            _buff = _buff->link;
        }
        else
        {
            _buff->data = (uint8_t*)malloc(length);
            if (_buff->data == BUFF_NULL)
            {
                return BUFF_NOT_OK;
            }

            for (uint16_t countByte = 0; countByte < length; countByte++)
            {
                _buff->data[countByte] = u8DataPtr[countByte];
            }
            _buff->length = length;
            return BUFF_OK;
        }
    }

    _buff = (Buffer*)malloc(sizeof(Buffer));
    if (_buff == BUFF_NULL)
    {
        return BUFF_NOT_OK;
    }

    _buff->data = (uint8_t*)malloc(length);
    if (_buff->data == BUFF_NULL)
    {
        free(_buff);
        return BUFF_NOT_OK;
    }

    _buff->length = 0;
    _buff->link = BUFF_NULL;
    if (head_buff != BUFF_NULL)
    {
        head_buff->link = _buff;
    }

    for (uint16_t countByte = 0; countByte < length; countByte++)
    {
        _buff->data[countByte] = u8DataPtr[countByte];
    }
    _buff->length = length;
    return BUFF_OK;
}

uint8_t* Buffer_GetPtrData(Buffer* buff, uint16_t position)
{
    Buffer* _buff = buff;
    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count)
    {
        return BUFF_NULL;
    }

    for (uint16_t countData = 0; countData < position; countData++)
    {
        _buff = _buff->link;
    }
    return _buff->data;
}

uint16_t Buffer_GetSizeData(Buffer* buff, uint16_t position)
{
    Buffer* _buff = buff;
    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count)
    {
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < position; countData++)
    {
        _buff = _buff->link;
    }

    return _buff->length;
}

Buff_ReturnType Buffer_GetData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t* length)
{
    Buffer* _buff = buff;
    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count)
    {
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < position; countData++)
    {
        _buff = _buff->link;
    }

    for (uint16_t countByte = 0; countByte < _buff->length; countByte++)
    {
        u8DataPtr[countByte] = _buff->data[countByte];
    }
    *length = _buff->length;
    return BUFF_OK;
}

Buff_ReturnType Buffer_AppendData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t length)
{
    Buffer* _buff = buff;
    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count)
    {
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < position; countData++)
    {
        _buff = _buff->link;
    }

    uint8_t* newdata = (uint8_t*)realloc(_buff->data, _buff->length + length);
    if (newdata == BUFF_NULL)
    {
        return BUFF_NOT_OK;
    }
    _buff->data = newdata;

    for (uint16_t countByte = 0; countByte < length; countByte++)
    {
        _buff->data[_buff->length + countByte] = u8DataPtr[countByte];
    }
    _buff->length = _buff->length + length;

    return BUFF_OK;
}

Buff_ReturnType Buffer_ReplaceData(Buffer* buff, uint16_t position, uint8_t* u8DataPtr, uint16_t length)
{
    Buffer* currbuff = buff;
    Buffer* headbuff = buff;
    Buffer* tailbuff = buff;

    Buffer* newbuff = (Buffer*)malloc(sizeof(Buffer));
    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count || newbuff == BUFF_NULL)
    {
        return BUFF_NOT_OK;
    }
    for (uint16_t countData = 0; countData < position; countData++)
    {
        headbuff = currbuff;
        if (currbuff != BUFF_NULL)
        {
            currbuff = currbuff->link;
            tailbuff = currbuff->link;
        }
    }

    newbuff->data = (uint8_t*)malloc(length);
    if (newbuff->data == BUFF_NULL)
    {
        free(newbuff);
        return BUFF_NOT_OK;
    }

    for (uint16_t countByte = 0; countByte < length; countByte++)
    {
        newbuff->data[countByte] = u8DataPtr[countByte];
    }
    newbuff->length = length;
    newbuff->link = (tailbuff != currbuff) ? tailbuff : BUFF_NULL;

    if (headbuff != currbuff)
    {
        headbuff->link = newbuff;
    }
    else
    {
        buff = newbuff;
    }

    if (currbuff != BUFF_NULL)
    {
        free(currbuff->data);
        free(currbuff);
    }
    return BUFF_OK;
}

Buff_ReturnType array_equal(const uint8_t* array1, uint16_t size1, const uint8_t* array2, uint16_t size2)
{
    int flag = BUFF_OK;

    if (size1 != size2)
    {
        return flag = BUFF_NOT_OK;
    }

    for (uint16_t i = 0; i < size1; ++i)
    {
        if (array1[i] != array2[i])
        {
            return flag = BUFF_NOT_OK;
        }
    }
    return flag;
}

Buff_ReturnType Buffer_SearchData(Buffer* buff, uint8_t* u8DataPtr, uint16_t length, uint16_t* position)
{
    Buffer* _buff = buff;
    uint16_t pos = 0;

    while (_buff != BUFF_NULL)
    {
        if (BUFF_OK == array_equal(_buff->data, _buff->length, u8DataPtr, length))
        {
            *position = pos;
            return BUFF_OK;
        }

        pos++;
        _buff = _buff->link;
    }
    pos = 0xFFFF;
    return BUFF_NOT_OK;
}


Buff_ReturnType Buffer_SplitData(Buffer* buff, uint16_t posBuff, uint16_t posData)
{
    Buffer* _buff = buff;
    Buffer* next_buff = _buff->link;

    uint16_t Count = Buffer_CountData(buff);

    if (posBuff >= Count)
    {
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < posBuff; countData++)
    {
        _buff = _buff->link;
        next_buff = _buff->link;
    }

    if (posData == 0 || posData > _buff->length)
    {
        return BUFF_NOT_OK;
    }

    uint8_t* newdatahead = (uint8_t*)malloc(posData - 1);
    uint8_t* newdatatail = (uint8_t*)malloc(_buff->length - posData + 1);
    Buffer* newbuff = newBuffer();

    if (newdatahead == BUFF_NULL || newdatatail == BUFF_NULL || newbuff == BUFF_NULL)
    {
        free(newdatahead);
        free(newdatatail);
        free(newbuff);
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < _buff->length; countData++)
    {
        if (countData < (posData - 1))
        {
            newdatahead[countData] = _buff->data[countData];
        }
        else
        {
            newdatatail[countData - posData + 1] = _buff->data[countData];
        }
    }

    newbuff->data = newdatatail;
    newbuff->length = _buff->length - posData + 1;
    newbuff->link = next_buff;

    free(_buff->data);
    _buff->data = newdatahead;
    _buff->length = posData - 1;
    _buff->link = newbuff;

    return BUFF_OK;
}

Buff_ReturnType Buffer_MergeData(Buffer* buff, uint16_t posSrc, uint16_t posDes)
{

    return BUFF_OK;
}

Buff_ReturnType Buffer_MergeAllData(Buffer* buff)
{
    Buffer* _buff = buff->link;


    while (_buff != BUFF_NULL)
    {
        uint8_t *newdata = (uint8_t*)realloc(buff->data, buff->length + _buff->length);
        if (newdata == BUFF_NULL)
        {
            return BUFF_NOT_OK;
        }
        buff->data = newdata;
        for (uint16_t countByte = 0; countByte < _buff->length; countByte++)
        {
            newdata[buff->length + countByte] = _buff->data[countByte];
        }
        buff->length += _buff->length;
        _buff = _buff->link;
    }
    
    Buffer_Dispose(buff->link);
    buff->link = BUFF_NULL;
    return BUFF_OK;
}

Buff_ReturnType Buffer_DeleteData(Buffer* buff, uint16_t position)
{
    Buffer* lastbuff = buff;
    Buffer* currbuff = buff;

    uint16_t Count = Buffer_CountData(buff);

    if (position >= Count)
    {
        return BUFF_NOT_OK;
    }

    for (uint16_t countData = 0; countData < (Count - 1); countData++)
    {
        if (countData >= position)
        {
            if (currbuff->data != BUFF_NULL && countData == position)
            {
                free(currbuff->data);
                currbuff->data = NULL;
            }
            currbuff->data = currbuff->link->data;
            currbuff->length = currbuff->link->length;
        }
        lastbuff = currbuff;
        currbuff = currbuff->link;       
    }

    lastbuff->link = BUFF_NULL;
    if (Count == 1)
    {
        if (currbuff->data != BUFF_NULL)
        {
            free(currbuff->data);
        }
        currbuff->data = BUFF_NULL;
        currbuff->length = 0;
        currbuff->link = BUFF_NULL;

    }
    else
    {
        free(currbuff);
        currbuff = NULL;
    }

    return BUFF_OK;
}

uint16_t Buffer_CountData(Buffer* buff)
{
    Buffer* _buff = buff;
    uint16_t countData = 0;

    while (_buff != BUFF_NULL)
    {
        countData++;
        _buff = _buff->link;
    }
    return countData;
}

void Buffer_Dispose(Buffer* buff)
{
    uint16_t nbData = Buffer_CountData(buff);
    for (uint16_t countData = 0; countData < nbData; countData++)
    {
        Buffer_DeleteData(buff, 0);
    }
    free(buff);
    buff = NULL;
}

Buffer* Buffer_SlipSplitPack(uint8_t TypeFrame, uint8_t* u8DataPtr, uint16_t length)
{
    uint8_t _data[2];
    Buffer* buff = newBuffer();
    if (TypeFrame == SLIP_FULLFRAME || TypeFrame == SLIP_HEADFRAME)
    {
        _data[0] = SLIP_END;
        if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
        {
            Buffer_Dispose(buff);
            return BUFF_NULL;
        }
    }

    for (uint16_t countByte = 0; countByte < length; ++countByte)
    {
        if (u8DataPtr[countByte] == SLIP_END)
        {
            _data[0] = SLIP_ESC;
            _data[1] = SLIP_ESC_END;
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 2))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
        else if (u8DataPtr[countByte] == SLIP_ESC)
        {
            _data[0] = SLIP_ESC;
            _data[1] = SLIP_ESC_ESC;
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 2))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
        else
        {
            _data[0] = u8DataPtr[countByte];
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
    }

    if (TypeFrame == SLIP_FULLFRAME || TypeFrame == SLIP_ENDFRAME)
    {
        _data[0] = SLIP_END;
        if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
        {
            Buffer_Dispose(buff);
            return BUFF_NULL;
        }
    }
    return buff;
}

Buffer* Buffer_SlipEnc(uint8_t* u8DataPtr, uint16_t length)
{
    uint8_t _data[2];
    Buffer* buff = newBuffer();

    _data[0] = SLIP_END;
    if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
    {
        Buffer_Dispose(buff);
        return BUFF_NULL;
    }

    for (uint16_t countByte = 0; countByte < length; ++countByte)
    {
        if (u8DataPtr[countByte] == SLIP_END)
        {
            _data[0] = SLIP_ESC;
            _data[1] = SLIP_ESC_END;
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 2))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
        else if (u8DataPtr[countByte] == SLIP_ESC)
        {
            _data[0] = SLIP_ESC;
            _data[1] = SLIP_ESC_ESC;
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 2))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
        else
        {
            _data[0] = u8DataPtr[countByte];
            if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
            {
                Buffer_Dispose(buff);
                return BUFF_NULL;
            }
        }
    }

    _data[0] = SLIP_END;
    if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, _data, 1))
    {
        Buffer_Dispose(buff);
        return BUFF_NULL;
    }
    return buff;
}

Buffer* Buffer_SlipDec(uint8_t* u8DataPtr, uint16_t length)
{
    Buffer* buff = newBuffer();
    uint8_t u8Data_Last = 0;
    uint8_t value = 0;

    if(length == 1)
    { 
    	Buffer_Dispose(buff);
        return BUFF_NULL;
    }

    for (uint16_t countByte = 0; countByte < length; ++countByte)
    {
        if (u8DataPtr[countByte] == SLIP_ESC)
        {
            u8Data_Last = SLIP_ESC;
        }
        else if (u8DataPtr[countByte] == SLIP_END)
        {
            if (countByte == (length - 1))
            {
                return buff;
            }
            else
            {
                Buffer_DeleteData(buff, 0);
            }
        }
        else
        {
            if (u8Data_Last == SLIP_ESC)
            {
                u8Data_Last = 0U;
                if (u8DataPtr[countByte] == SLIP_ESC_END)
                {
                    value = SLIP_END;
                    if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, &value, 1))
                    {
                        Buffer_Dispose(buff);
                        return BUFF_NULL;
                    }
                }
                else if (u8DataPtr[countByte] == SLIP_ESC_ESC)
                {
                    value = SLIP_ESC;
                    if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, &value, 1))
                    {
                        Buffer_Dispose(buff);
                        return BUFF_NULL;
                    }
                }
                else
                {
                    Buffer_Dispose(buff);
                    return BUFF_NULL;
                }
            }
            else
            {
                if (BUFF_NOT_OK == Buffer_AppendData(buff, 0, &u8DataPtr[countByte], 1))
                {
                    Buffer_Dispose(buff);
                    return BUFF_NULL;
                }
            }
        }
    }

    Buffer_Dispose(buff);
    return BUFF_NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
uint16_t Ring_CountByte(Ring *_ring)
{
    uint16_t count = 0;
    Ring* ring = _ring;
    if (ring->tail >= ring->head)
    {
        count = ring->tail - ring->head;
    }
    else
    {
        count = ring->size - ring->head + ring->tail;
    }
    return count;
}

uint16_t Ring_CountBytePos(Ring *_ring, uint16_t Pos)
{
    uint16_t count = 0;
    uint16_t rt_pos = Pos + 1;
    Ring* ring = _ring;

    if (rt_pos == ring->size)
    {
        rt_pos = 0;
    }
    else if (rt_pos > ring->size)
    {
        return 0;
    }

    if (rt_pos >= ring->head)
    {
        count = rt_pos - ring->head;
    }
    else
    {
        count = ring->size - ring->head + rt_pos;
    }
    return count;
}

Buff_ReturnType Ring_DeleteData(Ring* _ring, uint16_t Pos)
{
    if (_ring->tail > _ring->head)
    {
        if (Pos >= _ring->head && Pos < _ring->tail)
        {
            _ring->head = ((Pos + 1) >= _ring->size) ? 0 : (Pos + 1);
            return BUFF_OK;
        }
        else
        {
            return BUFF_NOT_OK;
        }
    }
    else
    {
        if (Pos >= _ring->head || Pos < _ring->tail)
        {
            _ring->head = ((Pos + 1) >= _ring->size) ? 0 : (Pos + 1);
            return BUFF_OK;
        }
        else
        {
            return BUFF_NOT_OK;
        }
    }


}

Buff_ReturnType Ring_PushData(Ring* _ring, uint8_t* data, uint16_t length)
{

    for (uint16_t CountByte = 0; CountByte < length; CountByte++)
    {
        _ring->data[_ring->tail] = data[CountByte];
        if (++_ring->tail == _ring->size)
        {
            _ring->tail = 0;
        }

        if (_ring->tail == _ring->head)
        {
            if (--_ring->tail < 0)
            {
                _ring->tail = _ring->size - 1;
            }
            _ring->data[_ring->tail - 1] = data[CountByte];
        }
    }
    return BUFF_OK;
}

int16_t Ring_PullData(Ring* _ring, uint8_t* data, uint16_t length)
{
    uint16_t countData = Ring_CountByte(_ring);
    uint16_t maxByteRead = (countData >= length) ? length : countData;

    for (uint16_t CountByte = 0; CountByte < maxByteRead; CountByte++)
    {
        data[CountByte] = _ring->data[_ring->head];
        if (++_ring->head == _ring->size)
        {
            _ring->head = 0;
        }
    }
    return countData - length;
}

uint16_t Ring_FindByte(Ring *_ring, uint8_t Byte, uint16_t* pos)
{
    uint16_t PositionByte = _ring->head;
    if (_ring->head == _ring->tail)
    {
        return 0;
    }
    uint16_t countdata = Ring_CountByte(_ring);

    for (uint16_t CountByte = 0; CountByte < countdata; CountByte++)
    {
        if (_ring->data[PositionByte] == Byte)
        {
            *pos = PositionByte;
            return CountByte + 1;
        }

        if (++PositionByte == _ring->size)
        {
            PositionByte = 0;
        }
    }
    *pos = 0;
    return 0;
}

Buff_ReturnType Ring_GetData(Ring* _ring, uint8_t* data, uint16_t* length, uint16_t pos)
{
    uint16_t newhead = _ring->head;

    for (uint16_t CountByte = 0; CountByte < _ring->size; CountByte++)
    {
        data[CountByte] = _ring->data[newhead];
        if (newhead == pos)
        {
            *length = CountByte + 1;
            _ring->head = (newhead + 1) >= _ring->size ? 0 : (newhead + 1);

            if (pos == _ring->tail)
            {
                *length = CountByte;
                _ring->head = _ring->tail;
            }
            return BUFF_OK;
        }
        if (newhead == _ring->tail)
        {
            break;
        }

        if (++newhead >= _ring->size)
        {
            newhead = 0;
        }
    }
    return BUFF_NOT_OK;
}

Ring *newRing(uint16_t size)
{
    uint8_t* dataPtr = (uint8_t*)malloc(size);
    Ring* res = (Ring*)malloc(sizeof(Ring));

    res->size = size;
    res->head = 0;
    res->tail = 0;
    res->data = dataPtr;

    return res;
}

void Ring_Dispose(Ring* ring)
{
    if (ring->data != BUFF_NULL)
    {
        free(ring->data);
        ring->data = BUFF_NULL;
    }

    free(ring);
}

////////////////////////////////////////////////////////////////////////////////////


Buff_ReturnType Fifo_AddData(Fifo* fifo, void* data, uint16_t length)
{
    ListObj* headobj = fifo->objs;
    ListObj* objs = fifo->objs;

    Object  object;
    object.addr = data;
    object.length = length;

    if (fifo->size <= Fifo_CountData(fifo) || object.addr == BUFF_NULL)
    {
        return BUFF_NOT_OK;
    }

    while (objs != BUFF_NULL)
    {
        headobj = objs;
        objs = objs->next;
    }

    ListObj* newobj = (ListObj*)malloc(sizeof(ListObj));
    Object* obj = (Object*)malloc(sizeof(Object));
    void* addr = malloc(object.length);

    if (newobj == BUFF_NULL || obj == BUFF_NULL || addr == BUFF_NULL)
    {
        free(newobj);
        free(obj);
        return BUFF_NOT_OK;
    }

    obj->addr = addr;
    obj->length = object.length;
    newobj->obj = obj;
    newobj->next = BUFF_NULL;

    uint8_t* datades = (uint8_t*)newobj->obj->addr;
    uint8_t* datasrc = (uint8_t*)object.addr;

    for (uint16_t countByte = 0; countByte < object.length; countByte++)
    {
        datades[countByte] = datasrc[countByte];
    }

    if (headobj == BUFF_NULL)
    {
        fifo->objs = newobj;
    }
    else
    {
        headobj->next = newobj;
    }

    return BUFF_OK;
}

uint16_t Fifo_GetSizeData(Fifo* fifo)
{
    ListObj* objs = fifo->objs;
    if (objs == BUFF_NULL)
    {
        return 0;
    }
    return objs->obj->length;
}

Buff_ReturnType Fifo_GetData(Fifo* fifo, void* data, uint16_t* length)
{

    ListObj* objs = fifo->objs;

    if (data == BUFF_NULL)
    {
        return BUFF_NOT_OK;
    }

    if (objs != BUFF_NULL)
    {
        uint8_t* datades = (uint8_t*)data;
        uint8_t* datasrc = (uint8_t*)objs->obj->addr;

        for (uint16_t countByte = 0; countByte < objs->obj->length; countByte++)
        {
            datades[countByte] = datasrc[countByte];
        }
        *length = objs->obj->length;
        fifo->objs = fifo->objs->next;

        free(objs->obj->addr);
        free(objs->obj);
        free(objs);
    }
    else
    {
        return BUFF_NOT_OK;
    }

    return BUFF_OK;
}

uint16_t Fifo_CountData(Fifo *fifo)
{
    uint16_t countNode = 0;
    ListObj* objs = fifo->objs;

    while (objs != BUFF_NULL)
    {
        objs = objs->next;
        countNode++;
    }

    return countNode;
}

void Fifo_Dispose(Fifo* fifo)
{
    uint8_t* data;
    uint16_t NbNode = Fifo_CountData(fifo);
    for (uint16_t CountNode = 0; CountNode < NbNode; CountNode  ++)
    {
        uint16_t size = Fifo_GetSizeData(fifo);
        data = (uint8_t *)malloc(size);
        if (data != BUFF_NULL)
        {
            Fifo_GetData(fifo, data, &size);
            free(data);
            data = BUFF_NULL;
        }
    }

    if (fifo != BUFF_NULL)
    {
        free(fifo);
    }
}

Fifo* newFifo(uint16_t size)
{
    Fifo *res = (Fifo *)malloc(sizeof(Fifo));
    res->size = size;
    res->objs = BUFF_NULL;
    return res;
}

