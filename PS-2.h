#include <Arduino.h>
#include <PinRead.h>
#ifndef _PS_2_Lib_
#define _PS_2_Lib_
#define TIMEOUT 30000 //us
#define BIT_INTERVAL 1 //us
enum Error
{
	SUCCESS=0x00,                //�����շ��ɹ�
	CLOCK_DOWNTIMEOUT=0x01,      //�ȴ�ʱ���½��س�ʱ
	CLOCK_UPTIMEOUT=0x02,        //�ȴ�ʱ�������س�ʱ
	DATA_DOWNTIMEOUT = 0x03,     //�ȴ������½��س�ʱ
	DATA_UPTIMEOUT = 0x04,       //�ȴ����������س�ʱ
	PARITYBIT_CHECK=0x05,        //��żУ�����
	STOPBIT_CHECK=0X06,          //��ȡֹͣλ��ʱ
	DATA_SENDERROR=0x07          //���Դӻ����͵�����У�����
};
class PS_2{
public :
	PS_2(uint8_t da,uint8_t cl);
	~PS_2();
	Error ReadByte(uint8_t*data);
	Error WriteByte(uint8_t data);
	
	String GetLastError();
	PinRead Data;
	PinRead Clock;
protected:
	Error LastError = SUCCESS;
	void GetClock();
	void ReleaseClock();
private:
	Error WaitForClockUp();
	Error WaitForClockDown();
	Error WaitForDataUp();
	Error WaitForDataDown();
	void ResetDataAndClock();
	//uint8_t GetParity(uint8_t);
	//PinRead Data;
	//PinRead Clock;
};
#endif