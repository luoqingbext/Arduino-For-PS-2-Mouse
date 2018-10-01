#include "PS-2.h"
PS_2::PS_2(uint8_t da, uint8_t cl){
	Data.Init(da);
	Clock.Init(cl);
	Clock.SetToPullUp();
	Data.SetToPullUp();
	/*Clock.SetToOutput();
	Clock.PullDown();
	Data.SetToOutput();
	Data.PullUp();*/
}
PS_2::~PS_2()
{
	
}
String PS_2::GetLastError(){
	switch (LastError)
	{
	    case SUCCESS:
			return String("SUCCESS");
		case CLOCK_DOWNTIMEOUT:
			return String("CLOCK_DOWNTIMEOUT");
		case CLOCK_UPTIMEOUT:
			return String("CLOCK_UPTIMEOUT");
		case DATA_DOWNTIMEOUT:
			return String("DATA_DOWNTIMEOUT");
		case DATA_UPTIMEOUT:
			return String("DATA_UPTIMEOUT");
		case PARITYBIT_CHECK:
			return String("PARITYBIT_CHECK");
		case STOPBIT_CHECK:
			return String("STOPBIT_CHECK");
		case DATA_SENDERROR:
			return String("DATA_SENDERROR");
	}
}
void PS_2::ResetDataAndClock(){
	Clock.SetToOutput();
	Data.SetToOutput();
	Clock.PullDown();
	Data.PullUp();
}
Error PS_2::WaitForClockUp(){
	int t = 0;
	while (!Clock.GetInput()){
		if (++t > TIMEOUT) {
			return CLOCK_UPTIMEOUT;
		}
		Delay_us(BIT_INTERVAL);
	}
	return SUCCESS;
}
Error PS_2::WaitForClockDown(){
	int t = 0;
	while (Clock.GetInput()){
		if (++t>TIMEOUT) return CLOCK_DOWNTIMEOUT;
		Delay_us(BIT_INTERVAL);
	}
	return SUCCESS;
}
Error PS_2::WaitForDataUp(){
	int t = 0;
	while (!Data.GetInput()){
		if (++t > TIMEOUT) return DATA_UPTIMEOUT;
		Delay_us(BIT_INTERVAL);
	}
	
	return SUCCESS;
}
Error PS_2::WaitForDataDown()
{
	int t = 0;
	while (Data.GetInput()){
		if (++t>TIMEOUT) return DATA_DOWNTIMEOUT;
		Delay_us(BIT_INTERVAL);
	}
	return SUCCESS;
}
Error PS_2::ReadByte(uint8_t*data){
	*data = 0;
	Data.SetToPullUp();   //�Ƚ�����������
	Clock.SetToPullUp();
	if ((LastError=WaitForClockDown())!=SUCCESS) return LastError;
	if ((LastError=WaitForDataDown())!=SUCCESS) return LastError;  //�ȴ���귢����ʼλ
	if ((LastError=WaitForClockUp())!=SUCCESS) return LastError;
	//��ʼ���հ�λ����
	uint8_t i = 0, buf = 0;//buf Ϊ����λ��chk Ϊ��żУ��λ
	bool chk = true;
	while (i < 8){
		buf = buf >>1;
		if ((LastError=WaitForClockDown())!=SUCCESS) return LastError;
		if (Data.GetInput())
		{
			buf = buf | 0x80;
			chk = ~chk;
		}
		if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
		i++;
	}

	//����У��λ

	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	if (Data.GetInput() != chk) LastError = PARITYBIT_CHECK;
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
	//����ֹͣλ ֹͣλ����1
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	if (Data.GetInput()) {
		if (WaitForClockUp() != SUCCESS) return CLOCK_UPTIMEOUT;
		*data = buf;
		return LastError;
	
	}
	else
	    return STOPBIT_CHECK;
}
Error PS_2::WriteByte(uint8_t data){
	bool res,chk =true;
	Clock.SetToOutput();
	//��ȡ��������Ȩ ����ʱ���߲���ʱ200us
	Clock.PullDown();
	Delay_us(200);
	Data.SetToOutput();
	Data.PullDown();    //����ֹͣλ
	Clock.SetToPullUp();
	//���Ͱ�λ����
	for (int i = 0; i < 8; i++)
	{
		if ((LastError =WaitForClockDown()) != SUCCESS) return LastError;
		if ((data & 0x01) == 1){
			chk = !chk;
			Data.PullUp();
		}
		else Data.PullDown();
		data = data >>1;
		if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
	
	}
	//����У��λ
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	chk == true ? Data.PullUp() : Data.PullDown();
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;

	//����ֹͣλ
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	Data.SetToPullUp();

	//����ȷ��λ
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	res = Data.GetInput();
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
	if (res != 0) {
		return WriteByte(data);
	}
	else
	     return SUCCESS;
}
void PS_2::GetClock(){
	Clock.SetToOutput();
	Data.SetToOutput();
	Data.PullUp();
	Clock.PullDown();
}
void PS_2::ReleaseClock(){

}
