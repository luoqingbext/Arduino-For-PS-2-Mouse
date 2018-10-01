#include "PS-2-Mouse.h"
PS_2_MOUSE::PS_2_MOUSE(uint8_t da, uint8_t cl) :PS_2(da,cl)
{
	
}

PS_2_MOUSE::~PS_2_MOUSE()
{
}
String PS_2_MOUSE::GetPSLastError(){
	return PS_2::GetLastError();
}
String PS_2_MOUSE::GetLastError(){
	switch (Mouse_LastError)
	{
	    case MOUSE_SUCCESS:
			return String("MOUSE_SUCCESS");
		case ERROR_RESET:
			return String("ERROR_RESET");
		case ERROR_RESETTIMEOUT:
			return String("ERROR_RESETTIMEOUT");
		case ERROR_RESETRESPONSE1:
			return String("ERROR_RESETRESPONSE1");
		case ERROR_RESETRESPONSE2:
			return String("ERROR_RESETRESPONSE2");
		case ERROR_RESETRESPONSE3:
			return String("PARITYBIT_CHECK");
		case ERROR_SETRATE:
			return String("ERROR_SETRATE");
		case ERROR_SETRATEV:
			return String("ERROR_SETRATEV");
		case ERROR_SETRESOLUTION:
			return String("ERROR_SETRESOLUTION");
		case ERROR_GETDEVICEID:
			return String("ERROR_GETDEVICEID");
		case ERROR_SETSCALING:
			return String("ERROR_SETSCALING");
		case ERROR_ENREMOTE:
			return String("ERROR_ENREMOTE");
		case ERROR_ENWARP:
			return String("ERROR_ENWARP");
		case ERROR_ENSTREAM:
			return String("ERROR_ENSTREAM");
		case ERROR_GETSTATUS:
			return String("ERROR_GETSTATUS");
		case ERROR_GETDATA:
			return String("ERROR_GETDATA");
		default:
			return String("ERROR_UNKNOWN");
	}
}
Mouse_Error PS_2_MOUSE::WaitForMouse(){
	//byte buf = 0;
	do{
		READ3(CMD_RESET, buf, ACK_2, ERROR_RESETRESPONSE2);
	} while (LastError == CLOCK_DOWNTIMEOUT);
	    READ3(CMD_RESET,buf, ACK_3, ERROR_RESETRESPONSE3);
	return Mouse_LastError;
}
Mouse_Error PS_2_MOUSE::Init_Mouse(){
	//byte buf=0;

	//复位鼠标
	for (int i = 0; i < 3; i++)
	{
		if (WriteByte(CMD_RESET) != SUCCESS)   ERROR(ERROR_RESET);
		READ5(CMD_RESET,buf, ACK_1, ERROR_RESETRESPONSE1);
	    do{
			READ3(CMD_RESET, buf, ACK_2, ERROR_RESETRESPONSE2);
		} while (LastError == CLOCK_DOWNTIMEOUT);
		READ5(CMD_RESET,buf, ACK_3, ERROR_RESETRESPONSE3);
	}
	Serial.println("CMD_RESET");
	//鼠标的基本配置
	//设置采样率，设置采样率 200点/秒
	if (WriteByte(CMD_SETRATE) != SUCCESS)         ERROR(ERROR_SETRATE);
	   READ5(CMD_RESET,buf, ACK_1, ERROR_SETRATE);

    if (WriteByte(0xC8) != SUCCESS)         ERROR(ERROR_SETRATEV);
	READ5(0xC8, buf, ACK_1, ERROR_SETRATE);
	Serial.println("CMD_SETRATE");
	//设置采样率,设置采样率 100/秒
	if (WriteByte(CMD_SETRATE) != SUCCESS)         ERROR(ERROR_SETRATE);
	READ5(CMD_SETRATE,buf, ACK_1, ERROR_SETRATE);

	if (WriteByte(0x64) != SUCCESS)         ERROR(ERROR_SETRATEV);
	READ5(0x64,buf, ACK_1, ERROR_SETRATEV);

	//设置采样速率,设置采样速率 80/秒
	if (WriteByte(CMD_SETRATE) != SUCCESS)         ERROR(ERROR_SETRATE);
	READ5(CMD_SETRATE,buf, ACK_1, ERROR_SETRATE);

	if (WriteByte(0x50) != SUCCESS)         ERROR(ERROR_SETRATEV);
	READ5(0x50,buf, ACK_1, ERROR_SETRATEV);



	//获得设备ID
	if (WriteByte(CMD_GETDEVICEID) != SUCCESS)      ERROR(ERROR_GETDEVICEID);
	READ5(CMD_GETDEVICEID,buf,ACK_1, ERROR_GETDEVICEID);
	READ2(DeviceID, ERROR_GETDEVICEID);
	Serial.println("CMD_GETDEVICEID");

	//此段不知是什么意思
	//READ5(CMD_RESEND,buf,ACK_1, ERROR_UNKNOWN);
	if (WriteByte(CMD_SETRATE != SUCCESS) != SUCCESS)    ERROR(ERROR_SETRESOLUTION);
	READ5(CMD_SETRATE, buf, ACK_1, ERROR_UNKNOWN);
	if (WriteByte(0x0A) != SUCCESS)    ERROR(ERROR_UNKNOWN);  //采样速率 10点/秒
	READ5(0x0A, buf, ACK_1, ERROR_UNKNOWN);
	if (WriteByte(CMD_GETDEVICEID) != SUCCESS)      ERROR(ERROR_GETDEVICEID);
	READ5(CMD_GETDEVICEID, buf, ACK_1, ERROR_GETDEVICEID);
	READ2(DeviceID, ERROR_GETDEVICEID);
	Serial.println("CMD_UNKNOWN");


	//设置分辨率
	if (WriteByte(CMD_SETRESOLUTION) != SUCCESS)    ERROR(ERROR_SETRESOLUTION);
	READ5(CMD_SETRESOLUTION,buf, ACK_1, ERROR_SETRESOLUTION);

	if (WriteByte(0x03) != SUCCESS)    ERROR(ERROR_SETRESOLUTIONV);
	READ5(0x03,buf, ACK_1, ERROR_SETRESOLUTIONV);   //设置8个计数/mm
	Serial.println("CMD_SETRESOLUTION");

	//设置缩放比例1:1
	if (WriteByte(CMD_SETSCALING11) != SUCCESS)    ERROR(ERROR_SETSCALING);
	READ5(CMD_SETSCALING11,buf, ACK_1, ERROR_SETSCALING);
	Serial.println("CMD_SETSCALING11");

	//设置采样速率,设置采样速率 40/秒
	if (WriteByte(CMD_SETRATE) != SUCCESS)         ERROR(ERROR_SETRATE);
	READ5(CMD_SETRATE, buf, ACK_1, ERROR_SETRATE);

	if (WriteByte(0x28) != SUCCESS)         ERROR(ERROR_SETRATEV);
	READ5(0x50, buf, ACK_1, ERROR_SETRATEV);

	//设置到远程模式
	if (WriteByte(CMD_ENREMOTE) != SUCCESS)    ERROR(ERROR_ENREMOTE);
	READ5(CMD_ENREMOTE, buf, ACK_1, ERROR_ENREMOTE);
	Serial.println("CMD_ENREMOTE");
	return Mouse_LastError;
	
}

Mouse_Error PS_2_MOUSE::GetMouseStatus(){
	//uint8_t buf = 0;
	if (WriteByte(CMD_STATUSREQUEST) != SUCCESS)      ERROR(ERROR_GETSTATUS);
	READ5(CMD_READDATA,buf, ACK_1, ERROR_GETSTATUS);         //鼠标以0xFA应答
	READ2(Mouse_Status[0], ERROR_GETSTATUS);   //按键状态，模式
	READ2(Mouse_Status[1], ERROR_GETSTATUS);   //分辨率
	READ2(Mouse_Status[2], ERROR_GETSTATUS);   //采样速率
	return Mouse_LastError;
}

Mouse_Error PS_2_MOUSE::GetMouseData(){
	//uint8_t buf = 0;
	if (WriteByte(CMD_READDATA) != SUCCESS)      ERROR(ERROR_GETDATA);
	READ5(CMD_READDATA,buf, ACK_1, ERROR_GETDATA);       //鼠标以0xFA应答
	READ2(Mouse_Data[0], ERROR_GETDATA);   //按键状态
	READ2(Mouse_Data[1], ERROR_GETDATA);   //X位移
	READ2(Mouse_Data[2], ERROR_GETDATA);   //Y位移
	READ2(Mouse_Data[3], ERROR_GETDATA);   //标准鼠标：滚轮位移   五键鼠标：其他按键状态
	CalculateMove();
	return Mouse_LastError;
}

/*****************************获取各个状态位**************************************/
//获取左键状态  0:没按下 1:按下  
bool PS_2_MOUSE::GetLBNStatus(){
	return Mouse_Status[0] & MASK_LEFTBN;
}
//获取中键状态  0:没按下 1:按下    
bool PS_2_MOUSE::GetMBNStatus(){
	return Mouse_Status[0] & MASK_MIDDLEBN;
}
//获取右键状态  0:没按下 1:按下   
bool PS_2_MOUSE::GetRBNStatus(){
	return Mouse_Status[0] & MASK_RIGHTBN;
}
//获取模式状态  0:Stream 1:Remote
bool PS_2_MOUSE::GetModeStatus(){
	return Mouse_Status[0] & MASK_MODEL;
}
//获取数据报告状态 1:使能 0：未使能
bool PS_2_MOUSE::GetDataReportStatus()
{
	return Mouse_Status[0] & MASK_ENABLE;
}
//获取缩放比例状态 1: 2:1  0:  1:1
bool PS_2_MOUSE::GetScalingStatus(){
	return Mouse_Status[0] & MASK_SCALING;
}

/*****************************获取各个数据位**************************************/
//获取左键数据
bool PS_2_MOUSE::GetLBNData(){
	return Mouse_Data[0] & MASK_LBN;
}
//获取中键数据
bool PS_2_MOUSE::GetMBNData(){
	return Mouse_Data[0] & MASK_MBN;
}
//获取右键数据
bool PS_2_MOUSE::GetRBNData(){
	return Mouse_Data[0] & MASK_RBN;
}
bool PS_2_MOUSE::GetXOverFlow(){
	return Mouse_Data[0] & MASK_XOVERFLOW;
}
bool PS_2_MOUSE::GetYOverFlow(){
	return Mouse_Data[0] & MASK_YOVERFLOW;
}

int PS_2_MOUSE::GetXSign(){
	return (Mouse_Data[0] & MASK_XSIGNBIT )== 0 ? 1 : -1;
}
int PS_2_MOUSE::GetYSign(){
	return (Mouse_Data[0] & MASK_YSIGNBIT) == 0 ? 1 : -1;
}

int PS_2_MOUSE::GetX(){
	int buf = 0;
	if (GetXSign() == -1)
	{
		buf = (0x7F & (~Mouse_Data[1]))*(-1);
	}
	else buf |= Mouse_Data[1];
	return buf;
}
int PS_2_MOUSE::GetY(){
	int buf = 0;
	if (GetYSign() == -1)
	{
		buf = (0x7F & (~Mouse_Data[2]))*(-1);
	}
	else buf |= Mouse_Data[2];
	return buf;
}
int PS_2_MOUSE::GetZ(){
	return (Mouse_Data[3] & 0x80 ? -1 : 1)*(Mouse_Data[3] & 0x7F);
}
double PS_2_MOUSE::GetMoveX(){
	return X;
}
double PS_2_MOUSE::GetMoveY(){
	return Y;
}
double PS_2_MOUSE::GetMoveZ(){
	return Z;
}
uint8_t PS_2_MOUSE::GetLastByte(){
	return buf;
}
uint8_t PS_2_MOUSE::GetDeviceID(){
	return DeviceID;
}
uint8_t PS_2_MOUSE::GetDeviceDATA(uint8_t num){
	return Mouse_Data[num];
}
uint8_t PS_2_MOUSE::GetDeviceSTATUS(uint8_t num){
	return Mouse_Status[num];
}
void PS_2_MOUSE::CalculateMove(){
	X += (double(GetX()) / 8.0);
	Y += (double(GetY()) / 8.0);
	Z += double(GetZ()) / 8.0;
}
void PS_2_MOUSE::ResetMove(){
	X = 0;
	Y = 0;
	Z = 0;
}