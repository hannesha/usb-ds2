#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include "Descriptor.h"
#include "Cpp_Glue.h"

#define PIN(x)	(1<<x)
#define FIND_MIN(X,Y) (X)<(Y) ? (X) : (Y)

#define P_CLK		PIN(1)
#define P_MOSI	PIN(2)
#define P_MISO	PIN(3)
#define ATT		PIN(6)

#define READ_DELAY 5

void HW_Setup();

void SPI_MasterInit();
char SPI_Transmit(char);

// debug functions
void UART_Init(unsigned char);
void UART_Transmit(char);

// DS2 header struct
struct S_Header{
	char len:4, u:4;
	char ret;
};

// DS2 data struct
struct S_Data{
	char sel:1, joyl:1, joyr:1, start:1, hat:4;
	char l2:1, r2:1, l1:1, r1:1, tri:1,  O:1, X:1, sqr:1;
	char joyr_x = 128;
	char joyr_y = 128;
	char joyl_x = 128;
	char joyl_y = 128;
};

enum class DS2_State {
	RUNNING,
	SETUP,
	CONFIGURING,
	EXIT_SETUP
};

void read_DS2(S_Data*);

// hat switch(D pad) angle lookup table
const char ang_DP[16] = {0, 1, 3, 2, 5, 0, 4, 0, 7, 8, 0, 0, 6, 0, 0, 0};
