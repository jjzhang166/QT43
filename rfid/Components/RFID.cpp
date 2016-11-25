#include "RFID.h"
#include <QThread>
#include <stdint.h>
#include <string>
#define SWAP16(x)    (((x)<<8) | ((x)>>8))
#define SIZE(x)    (x)

RFID::RFID(void)
{
    Retry = 15;
}

RFID::~RFID(void)
{
	Close();
}

bool RFID::Open(const char *name, int br)
{
	bool ret = true;

    Dev.setPortName(name);
    Dev.setBaudRate(br);
    Dev.setDataBits(QSerialPort::Data8);
    Dev.setParity(QSerialPort::NoParity);
    Dev.setStopBits(QSerialPort::OneStop);
    Dev.setFlowControl(QSerialPort::NoFlowControl);

    ret = Dev.open(QIODevice::ReadWrite);

    return ret;
}

void RFID::Close()
{
    Dev.close();
}

int RFID::Read(unsigned char *buf, short size)
{
    int len = 0;
    char *pbuf = (char*)buf;
    int cnt = 0;

    while (len < size)
    {
        int n;

        n = Dev.read(pbuf, size - len);

        if (n == -1)
            break;

        if (n == 0)
        {
            Dev.waitForReadyRead(20);
            if (cnt ++ > Retry)
                break;

            continue;
        }

        buf += n;
        len += n;
        cnt = 0;
    }

    return len;
}

int RFID::Write(unsigned char *buf, short size)
{
	int len;

    Dev.clear();
    len = Dev.write((char*)buf, size);
    buf[len] = 0;

    std::string str;
    for (int i = 0; i < len; i++)
    {
        char tmp[4];

        sprintf(tmp, "%02X", buf[i]);
        str += tmp;
        str += " ";
    }

    qDebug("%s", str.c_str());
    return len; 
}

bool RFID::ReqSend(short cmd, unsigned char *data, short size)
{
    unsigned char buf[128];
    zm704_hdr_t *pkt;
	int len;

	pkt = (zm704_hdr_t*)buf;

	pkt->flag = (unsigned short)ZM704_FLAG;
    pkt->addr = 0xAA;
	pkt->cmd  = SWAP16(cmd);
    pkt->len  = SIZE(size + 3);

	if ((data != NULL) && (size != 0))
	{
        memcpy(pkt->data, data, size);
	}

	len = sizeof(zm704_hdr_t) + size;
	pkt->data[size] = Crc8(buf, len - 1);
    
	Write(buf, len);

	return true;
}

bool RFID::Alive(void)
{
    uint8_t mode[1];
	int size;
    uint8_t buf[32];
	bool ret = true;

	mode[0] = 0x26;

	ReqSend(ZM_CMD_SCAN_CARD_AUTO, mode, 1);

	size = Read(buf, 15);
    Read(buf, 15);
    Dev.clear();

	if (size == 0)
	{
        ret = false;
	}
	
	return ret;
}

void RFID::SetRetry(int retry)
{
    Retry = retry;
}

bool RFID::CardScan()
{
    uint8_t buf[15];
    bool ret = false;
    uint8_t mode[1] = {0x26};

    //寻卡
    ReqSend(ZM_CMD_SCAN_CARD_AUTO, NULL, 0);
    if (AckRecv(buf, 1) == 0)
        return ret;
    if (buf[0] != 0xFF)
        return ret;
#if 0
    ReqSend(ZM_CMD_GET_INFO, mode, 1);
    if (AckRecv(buf, 1) == 0)
        return ret;
    if (buf[0] != 0xFF)
        return ret;
#endif
    return true;
}

bool RFID::BlockRead(unsigned char blkn, unsigned char *buf, short size)
{
    uint8_t tmp[32] = {0};

    ReqSend(ZM_CMD_READ_BLOCK, &blkn, 1);

    if (AckRecv(tmp, sizeof(tmp)) == 0)
        return false;
    if (tmp[0] != 0xFF)
        return false;

    memcpy(buf, &tmp[1], size);

	return true;
}

bool RFID::Authen(unsigned char blkn, unsigned char type, unsigned char *pwd, short size)
{
    unsigned char buf[32];
	unsigned char section;

    section = blkn/4;

	buf[0] = type;
	buf[1] = section;
	memcpy(&buf[2], pwd, size);

    ReqSend(ZM_CMD_AUTHEN, buf, size + 2);

    if (AckRecv(buf, 1) == 0)
        return false;
    if (buf[0] != 0xFF)
        return false;

	return true;
}

bool RFID::CpuCardMode()
{
    unsigned char buf[32];

	ReqSend(ZM_CMD_ENTER_CPUCARD, NULL, 0);
    if (AckRecv(buf, 8) != 8)
        return false;

	return true;
}

bool RFID::CosSend(unsigned char *c, short size)
{
    return ReqSend(ZM_CMD_SEND_COS, c, size);
}

int RFID::AckRecv(unsigned char *buf, short bsize)
{
    unsigned char tmp[256] = {0};
    uint16_t len;
	short size;
    zm704_rsp_t *pkt;

    pkt = (zm704_rsp_t*)tmp;

    len = Read(tmp, sizeof(zm704_rsp_t));
    if (len < sizeof(zm704_rsp_t) || (pkt->flag != 0xAA55))
	{
		len = 0;
		goto FAIL;
	}

    len = SIZE(pkt->len) - 1;
    if (len > sizeof(tmp))
	{
		len = 0;
		goto FAIL;
	}

    size = Read(&tmp[sizeof(zm704_rsp_t)], len);
    size += (sizeof(zm704_rsp_t) - 1);

	if (tmp[size] != Crc8(tmp, size))
	{
		len = 0;
		goto FAIL;
	}

	if (len > bsize)
		len = bsize;

    if (buf != NULL && len > 0)
	{
        memcpy(buf, pkt->data, len);
	}

FAIL:

	return len;
}

unsigned char RFID::Crc8(unsigned char *buf, short len)
{
	unsigned char x1=0;
	unsigned char x2=0;
	unsigned char crc=0;

	if(buf == NULL)
		return 0;

	for(short i = 0; i < len; i ++)
	{
		x2 = buf[i];
		x1 ^= x2;
	}

	crc = x1;

	return crc;
}
