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
	Data.SetToPullUp();   //先将数据线拉高
	Clock.SetToPullUp();
	if ((LastError=WaitForClockDown())!=SUCCESS) return LastError;
	if ((LastError=WaitForDataDown())!=SUCCESS) return LastError;  //等待鼠标发送起始位
	if ((LastError=WaitForClockUp())!=SUCCESS) return LastError;
	//开始接收八位数据
	uint8_t i = 0, buf = 0;//buf 为数据位，chk 为奇偶校验位
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

	//接收校验位

	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	if (Data.GetInput() != chk) LastError = PARITYBIT_CHECK;
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;
	//接收停止位 停止位总是1
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
	//获取总线所有权 拉低时钟线并延时200us
	Clock.PullDown();
	Delay_us(200);
	Data.SetToOutput();
	Data.PullDown();    //发送停止位
	Clock.SetToPullUp();
	//发送八位数据
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
	//发送校验位
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	chk == true ? Data.PullUp() : Data.PullDown();
	if ((LastError = WaitForClockUp()) != SUCCESS) return LastError;

	//发送停止位
	if ((LastError = WaitForClockDown()) != SUCCESS) return LastError;
	Data.SetToPullUp();

	//接收确认位
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
