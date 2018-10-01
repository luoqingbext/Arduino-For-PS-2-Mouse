#include "PS-2.h"
#ifndef _PS_2_MOUSE_
#define _PS_2_MOUSE_
#define CMD_RESET            0xFF   //��긴λ����
#define CMD_RESEND           0xFE   //Ҫ��������·�������
#define CMD_DEFAULT          0xF6   //������굽Ĭ��ֵ
#define CMD_DISSTREAM        0xF5   //ֹͣ���STREAMģʽ�������ݱ���
#define CMD_ENSTREAM         0xF4   //�������STREAMģʽ�������ݱ���
#define CMD_SETRATE          0xF3   //���ò����� 
#define CMD_GETDEVICEID      0xF2   //����豸ID
#define CMD_ENWARP           0xEE   //���õ�WARPģʽ
#define CMD_DISWARP          0xEC   //��λWARPģʽ
#define CMD_READDATA         0xEB   //����������
#define CMD_SETTOSTREAM      0xEA   //������굽STREAMģʽ
#define CMD_STATUSREQUEST    0xE9   //���״̬����
#define CMD_SETRESOLUTION    0xE8   //���÷ֱ���
#define CMD_SETSCALING21     0xE7   //����X:Y Ϊ2:1
#define CMD_SETSCALING11     0xE6   //����X:Y Ϊ1:1
#define CMD_ENREMOTE         0xF0

#define MASK_ENABLE          0x20   //״̬����  1���ݱ��汻ʹ�� 0���ݱ��汻��ֹ
#define MASK_MODEL           0x40   //ģʽ����  1��ʾREMOTE    0��ʾSTREAM
#define MASK_SCALING         0x10   //���ű���  1��ʾ2:1   0��ʾ1:1
#define MASK_LEFTBN          0x04   //���״̬  1��ʾ����  0��ʾû�а���
#define MASK_MIDDLEBN        0x02   //�м�״̬
#define MASK_RIGHTBN         0x01   //�Ҽ�״̬

#define MASK_YOVERFLOW       0x80   //Y�������
#define MASK_XOVERFLOW       0x40   //X�������
#define MASK_YSIGNBIT        0x20   //Y�ź�BIT
#define MASK_XSIGNBIT        0x10   //X�ź�BIT
#define MASK_MBN             0x04   //�м�״̬
#define MASK_RBN             0x02   //�Ҽ�״̬
#define MASK_LBN             0x01   //���״̬

#define ACK_1                0xFA   //�����Ӧ����1
#define ACK_2                0xAA   //�����Ӧ����2
#define ACK_3                0x00   //�����Ӧ����3

#define MOUSE                0x00   //�޹������
#define WHEEL_MOUSE          0x03   //�������
enum Mouse_Error{
	MOUSE_SUCCESS=0x00,
	ERROR_RESET = 0x08,               //��긴λ����             
	ERROR_RESETTIMEOUT = 0x09,        //��긴λ��Ӧ���� 
	ERROR_RESETRESPONSE1 = 0x0A,      //��긴λ��Ӧ���� 
	ERROR_RESETRESPONSE2 = 0x0B,      //��긴λ��Ӧ���� 
	ERROR_RESETRESPONSE3 = 0x0C,      //��긴λ��Ӧ����
	ERROR_SETRATE = 0x0D,             //���ò����ʴ���
	ERROR_SETRATEV =0x0E,             //����������ֵʱ����
	ERROR_SETRESOLUTION=0x0F,         //���÷ֱ���ʱ����
	ERROR_SETRESOLUTIONV=0x10,        //�����ֱ���ֵʱ����
	ERROR_GETDEVICEID=0x11,           //��ȡ�豸ID����
	ERROR_SETSCALING=0x12,            //�������ű�������
	ERROR_ENREMOTE=0x13,              //���õ�Զ��ģʽ����
	ERROR_ENWARP=0x14,                //����WRAPģʽ����
	ERROR_ENSTREAM=0x15,              //���õ�ENSTREAMģʽ����
	ERROR_GETSTATUS=0x16,             //��ȡ�豸״̬����
	ERROR_GETDATA=0x17,               //��ȡ�豸���ݳ���
	ERROR_UNKNOWN=0x18                //δ֪����
};
enum Resolution
{
	Resolution_1 = 0x00,            //1 ����ֵ/����
	Resolution_2 = 0x01,            //2 ����ֵ/����
	Resolution_4 = 0x02,            //4 ����ֵ/����
	Resolution_8 = 0x08             //8 ����ֵ/����

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
	Mouse_Error Init_Mouse();     //��ʼ�����
	Mouse_Error GetMouseData();   //��ȡ�������
	Mouse_Error GetMouseStatus(); //��ȡ���״̬
	Mouse_Error WaitForMouse();   //�ȴ��������
	bool GetRBNStatus();          //��ȡ�Ҽ�״̬  0:û���� 1:����       
	bool GetLBNStatus();          //��ȡ���״̬  0:û���� 1:����     
	bool GetMBNStatus();          //��ȡ�м�״̬  0:û���� 1:����     
	bool GetModeStatus();         //��ȡģʽ״̬  0:Stream 1:Remote
	bool GetDataReportStatus();   //��ȡ���ݱ���״̬ 1:ʹ�� 0��δʹ��
	bool GetScalingStatus();      //��ȡ���ű���״̬ 1: 2:1  0:  1:1

	bool GetRBNData();            //��ȡ�Ҽ�����
	bool GetLBNData();            //��ȡ�������
	bool GetMBNData();            //��ȡ�м�����
	bool GetYOverFlow();          //Y�������λ
	bool GetXOverFlow();          //X�������λ
	int GetXSign();               //Xλ�����ķ���λ
	int GetYSign();               //Yλ�����ķ���λ
	int GetZSign();               //Zλ�����ķ���λ
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
	void CalculateMove();        //���������ƶ�
	uint8_t Mouse_Status[3] = { 0 };
	uint8_t Mouse_Data[4] = { 0 };
	uint8_t DeviceID = 0;
	//byte MouseID = 0;
	uint8_t buf = 0;
	double X, Y, Z;

};
#endif