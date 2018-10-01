#include <Arduino.h>
#include <PinRead.h>
#ifndef _PS_2_Lib_
#define _PS_2_Lib_
#define TIMEOUT 30000 //us
#define BIT_INTERVAL 1 //us
enum Error
{
	SUCCESS=0x00,                //数据收发成功
	CLOCK_DOWNTIMEOUT=0x01,      //等待时钟下降沿超时
	CLOCK_UPTIMEOUT=0x02,        //等待时钟上升沿超时
	DATA_DOWNTIMEOUT = 0x03,     //等待数据下降沿超时
	DATA_UPTIMEOUT = 0x04,       //等待数据上升沿超时
	PARITYBIT_CHECK=0x05,        //奇偶校验错误
	STOPBIT_CHECK=0X06,          //获取停止位超时
	DATA_SENDERROR=0x07          //来自从机发送的数据校验错误
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