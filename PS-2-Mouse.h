#include "PS-2.h"
#ifndef _PS_2_MOUSE_
#define _PS_2_MOUSE_
#define CMD_RESET            0xFF   //鼠标复位命令
#define CMD_RESEND           0xFE   //要求鼠标重新发送数据
#define CMD_DEFAULT          0xF6   //设置鼠标到默认值
#define CMD_DISSTREAM        0xF5   //停止鼠标STREAM模式发送数据报告
#define CMD_ENSTREAM         0xF4   //启动鼠标STREAM模式发送数据报告
#define CMD_SETRATE          0xF3   //设置采样率 
#define CMD_GETDEVICEID      0xF2   //获得设备ID
#define CMD_ENWARP           0xEE   //设置到WARP模式
#define CMD_DISWARP          0xEC   //复位WARP模式
#define CMD_READDATA         0xEB   //读数据请求
#define CMD_SETTOSTREAM      0xEA   //设置鼠标到STREAM模式
#define CMD_STATUSREQUEST    0xE9   //鼠标状态请求
#define CMD_SETRESOLUTION    0xE8   //设置分辨率
#define CMD_SETSCALING21     0xE7   //设置X:Y 为2:1
#define CMD_SETSCALING11     0xE6   //设置X:Y 为1:1
#define CMD_ENREMOTE         0xF0

#define MASK_ENABLE          0x20   //状态掩码  1数据报告被使能 0数据报告被禁止
#define MASK_MODEL           0x40   //模式掩码  1表示REMOTE    0表示STREAM
#define MASK_SCALING         0x10   //缩放比例  1表示2:1   0表示1:1
#define MASK_LEFTBN          0x04   //左键状态  1表示按下  0表示没有按下
#define MASK_MIDDLEBN        0x02   //中键状态
#define MASK_RIGHTBN         0x01   //右键状态

#define MASK_YOVERFLOW       0x80   //Y计数溢出
#define MASK_XOVERFLOW       0x40   //X计数溢出
#define MASK_YSIGNBIT        0x20   //Y信号BIT
#define MASK_XSIGNBIT        0x10   //X信号BIT
#define MASK_MBN             0x04   //中键状态
#define MASK_RBN             0x02   //右键状态
#define MASK_LBN             0x01   //左键状态

#define ACK_1                0xFA   //鼠标响应数据1
#define ACK_2                0xAA   //鼠标响应数据2
#define ACK_3                0x00   //鼠标响应数据3

#define MOUSE                0x00   //无滚轮鼠标
#define WHEEL_MOUSE          0x03   //滚轮鼠标
enum Mouse_Error{
	MOUSE_SUCCESS=0x00,
	ERROR_RESET = 0x08,               //鼠标复位错误             
	ERROR_RESETTIMEOUT = 0x09,        //鼠标复位响应错误 
	ERROR_RESETRESPONSE1 = 0x0A,      //鼠标复位响应错误 
	ERROR_RESETRESPONSE2 = 0x0B,      //鼠标复位响应错误 
	ERROR_RESETRESPONSE3 = 0x0C,      //鼠标复位响应错误
	ERROR_SETRATE = 0x0D,             //设置采样率错误
	ERROR_SETRATEV =0x0E,             //给定采样率值时出错
	ERROR_SETRESOLUTION=0x0F,         //设置分辨率时出错
	ERROR_SETRESOLUTIONV=0x10,        //给定分辨率值时出错
	ERROR_GETDEVICEID=0x11,           //获取设备ID出错
	ERROR_SETSCALING=0x12,            //设置缩放比例出错
	ERROR_ENREMOTE=0x13,              //设置到远程模式出错
	ERROR_ENWARP=0x14,                //设置WRAP模式出错
	ERROR_ENSTREAM=0x15,              //设置到ENSTREAM模式出错
	ERROR_GETSTATUS=0x16,             //获取设备状态出错
	ERROR_GETDATA=0x17,               //获取设备数据出错
	ERROR_UNKNOWN=0x18                //未知错误
};
enum Resolution
{
	Resolution_1 = 0x00,            //1 计数值/毫米
	Resolution_2 = 0x01,            //2 计数值/毫米
	Resolution_4 = 0x02,            //4 计数值/毫米
	Resolution_8 = 0x08             //8 计数值/毫米

};

class PS_2_MOUSE :public PS_2
{
#define ERROR(err)   {Mouse_LastError=err; return Mouse_LastError ;} 
#define READ(CMD,buff,ACK,ERR)    do{  ReadByte(&buff);\
	                              if (LastError != SUCCESS && LastError != PARITYBIT_CHECK) return Mouse_LastError = ERR; \
								  else if (LastError == PARITYBIT_CHECK) WriteByte(CMD_RESEND); \
								  else if (buff == CMD_RESEND) WriteByte(CMD); \
								  else { Mouse_LastError = MOUSE_SUCCESS; break; } \
}while (1); \
	if (buff != ACK) { return  Mouse_LastError = ERR; } \


#define READ2(buff,ERR)      do{  ReadByte(&buff);\
	if (LastError != SUCCESS && LastError != PARITYBIT_CHECK) return Mouse_LastError = ERR; \
								  else if (LastError == PARITYBIT_CHECK) WriteByte(CMD_RESEND); \
								  else { Mouse_LastError = MOUSE_SUCCESS; break; } \
}while (1)

#define READ3(CMD,buff,ACK,ERR)  do{ ReadByte(&buff); \
	                             if (LastError != SUCCESS && LastError != PARITYBIT_CHECK) { Mouse_LastError = ERR; break; }\
								 else if (LastError == PARITYBIT_CHECK) WriteByte(CMD_RESEND); \
								 else if (buff == CMD_RESEND) WriteByte(CMD); \
								 else { Mouse_LastError = MOUSE_SUCCESS; break; } \
								} while (1); \
	                            if (buff != ACK)  Mouse_LastError = ERR; 

#define READ4(buff,ERR)        do{  ReadByte(&buff);\
	                               if (LastError != SUCCESS && LastError != PARITYBIT_CHECK)  { Mouse_LastError = ERR; break }\
								   else if (LastError == PARITYBIT_CHECK || buff != ACK) WriteByte(CMD_RESEND); \
								   else { Mouse_LastError = MOUSE_SUCCESS; break; } \
								}while (1)
#define READ5(CMD,buff,ACK,ERR)    do{READ3(CMD,buff,ACK,ERR);}while(Mouse_LastError==ERR);

							
public:
	PS_2_MOUSE(uint8_t da, uint8_t cl);
	~PS_2_MOUSE();
	Mouse_Error Init_Mouse();     //初始化鼠标
	Mouse_Error GetMouseData();   //获取鼠标数据
	Mouse_Error GetMouseStatus(); //获取鼠标状态
	Mouse_Error WaitForMouse();   //等待鼠标启动
	bool GetRBNStatus();          //获取右键状态  0:没按下 1:按下       
	bool GetLBNStatus();          //获取左键状态  0:没按下 1:按下     
	bool GetMBNStatus();          //获取中键状态  0:没按下 1:按下     
	bool GetModeStatus();         //获取模式状态  0:Stream 1:Remote
	bool GetDataReportStatus();   //获取数据报告状态 1:使能 0：未使能
	bool GetScalingStatus();      //获取缩放比例状态 1: 2:1  0:  1:1

	bool GetRBNData();            //获取右键数据
	bool GetLBNData();            //获取左键数据
	bool GetMBNData();            //获取中键数据
	bool GetYOverFlow();          //Y计数溢出位
	bool GetXOverFlow();          //X计数溢出位
	int GetXSign();               //X位移量的符号位
	int GetYSign();               //Y位移量的符号位
	int GetZSign();               //Z位移量的符号位
	int GetX();
	int GetY();
	int GetZ();
	double GetMoveX();
	double GetMoveY();
	double GetMoveZ();
	uint8_t GetLastByte();
	uint8_t GetDeviceID();
	uint8_t GetDeviceDATA(uint8_t num);
	uint8_t GetDeviceSTATUS(uint8_t num);
	String GetLastError();
	String GetPSLastError();
	void ResetMove();
protected:
	Mouse_Error Mouse_LastError=MOUSE_SUCCESS;
private:
	void CalculateMove();        //计算鼠标的移动
	uint8_t Mouse_Status[3] = { 0 };
	uint8_t Mouse_Data[4] = { 0 };
	uint8_t DeviceID = 0;
	//byte MouseID = 0;
	uint8_t buf = 0;
	double X, Y, Z;

};
#endif