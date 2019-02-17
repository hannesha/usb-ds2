#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include "USBds2.hpp"

void SPI_MasterInit(){
	// set spi outputs
	DDRB |= P_MOSI | P_CLK;
	// enable spi with reversed bit order, fspi = fcpu/128
	SPCR = PIN(SPE) | PIN(DORD) | PIN(MSTR) | PIN(CPOL) | PIN(CPHA) | PIN(SPR1) | PIN(SPR0);
}

char SPI_Transmit(char data){
	SPDR = data;
	// wait for transmission
	while(!(SPSR & (1<<SPIF)));
	// return read buffer
	return SPDR;
}

void UART_Transmit(unsigned char data){
	while( !(UCSR1A & PIN(UDRE1)) );

	UDR1 = data;
}

void UART_Init(unsigned int baud){
	PRR0 &= ~PIN(PRSPI);
	DDRD = PIN(3);
	UBRR1H = (unsigned char)(baud >> 8);
	UBRR1L = (unsigned char)baud;

	UCSR1B = PIN(TXEN1);

	UCSR1C = 3<<UCSZ10;
}

DS2_State ds2_state = DS2_State::RUNNING;

void read_DS2(S_Data* data){
	PORTB &= ~ATT;

	S_Header header = {};
	char* pheader = reinterpret_cast<char*>(&header);

	// send Start Command
	SPI_Transmit(0x01);
	_delay_us(READ_DELAY);

	switch(ds2_state){
		case DS2_State::RUNNING:
			pheader[0] = SPI_Transmit(0x42);
			_delay_us(READ_DELAY);
			pheader[1] = SPI_Transmit(0x00);
			_delay_us(READ_DELAY);

			// enter setup if not in analog mode
			if(header.u != 7) ds2_state = DS2_State::SETUP; 

			// check header return value
			if(header.ret == 0x5A){
				// read controller data
				char* pdata = reinterpret_cast<char*>(data);
				unsigned char len = FIND_MIN((char)sizeof(S_Data), header.len*2);
				for(unsigned char i = 0; i < len; i++){
					pdata[i] = SPI_Transmit(0x00);
					_delay_us(READ_DELAY);
				}
				// invert button press values
				pdata[0] = ~pdata[0];
				pdata[1] = ~pdata[1];
			}
			break;

		case DS2_State::SETUP:
			pheader[0] = SPI_Transmit(0x43);
			_delay_us(READ_DELAY);
			pheader[1] = SPI_Transmit(0x00);
			_delay_us(READ_DELAY);
			
			if(header.ret == 0x5A){
				SPI_Transmit(0x01);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);

				ds2_state = DS2_State::CONFIGURING;
			}
			break;

		case DS2_State::CONFIGURING:
			pheader[0] = SPI_Transmit(0x44);
			_delay_us(READ_DELAY);
			pheader[1] = SPI_Transmit(0x00);
			_delay_us(READ_DELAY);
			
			if(header.ret == 0x5A){
				// lock controller in analog mode
				SPI_Transmit(0x01);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x03);
				_delay_us(READ_DELAY);

				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);

				ds2_state = DS2_State::EXIT_SETUP;
			}
			break;

		case DS2_State::EXIT_SETUP:
			pheader[0] = SPI_Transmit(0x43);
			_delay_us(READ_DELAY);
			pheader[1] = SPI_Transmit(0x00);
			_delay_us(READ_DELAY);
			
			if(header.ret == 0x5A){
				// lock controller in analog mode
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);

				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);
				SPI_Transmit(0x00);
				_delay_us(READ_DELAY);

				ds2_state = DS2_State::RUNNING;
			}
			break;
	}
	// clear ATT pin
	PORTB |= ATT;
}

void HW_Setup(){
	wdt_disable();
	clock_prescale_set(clock_div_1);
	USB_Init();

	SPI_MasterInit();
	// set ATT pin direction
	DDRB |=  ATT;

	GlobalInterruptEnable();

	//init uart at 9600 baud
	//UART_Init(104);
}

int main(){
	HW_Setup();

	for(;;){
		HID_Device_USBTask(&DS2_HID_Interface);
		USB_USBTask();
	}
}

void EVENT_USB_Device_ConfigurationChanged(void){
	bool config_success = true;
	config_success &= HID_Device_ConfigureEndpoints(&DS2_HID_Interface);

	USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest(void){
	HID_Device_ProcessControlRequest(&DS2_HID_Interface);
}

void EVENT_USB_Device_StartOfFrame(void){
	HID_Device_MillisecondElapsed(&DS2_HID_Interface);
}

inline char deadzone(char input){
	const unsigned char range = 10, middle = 128;
	if(input < (middle - range) && input > ((char)middle + range)){
		return input;
	}
	return middle;
}

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
	uint8_t* const ReportID,
	const uint8_t ReportType,
	void* ReportData,
	uint16_t* const ReportSize){

	DS2_Report* report = reinterpret_cast<DS2_Report*>(ReportData);
	// init controller data struct
	S_Data data = {};
	read_DS2(&data);

	report->joyr_x = deadzone(data.joyr_x);
	report->joyr_y = deadzone(data.joyr_y);
	report->joyl_x = deadzone(data.joyl_x);
	report->joyl_y = deadzone(data.joyl_y);
	// convert hat switch presses to angles
	report->hat = ang_DP[data.hat & 0x0F];

	report->b1  = data.X;
	report->b2  = data.O;
	report->b3  = data.sqr;
	report->b4  = data.tri;
	report->b5  = data.l1;
	report->b6  = data.r1;
	report->b7  = data.sel;
	report->b8  = data.start;
	report->b9  = data.joyl;
	report->b10 = data.joyr;
	report->b11 = data.l2;
	report->b12 = data.r2;
	
	*ReportSize = sizeof(DS2_Report);
	// always send report
	return true;
}

// don't process any host reports
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, const uint8_t ReportID, const uint8_t ReportType, const void* ReportData, const uint16_t ReportSize){}
