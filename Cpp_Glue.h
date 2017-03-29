#ifndef _CPP_GLUE_H_
#define _CPP_GLUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Descriptor.h"

typedef struct {
	char joyr_x;
	char joyr_y;
	char joyl_x;
	char joyl_y;
	char hat:4, b1:1, b2:1, b3:1, b4:1;
	char b5:1, b6:1, b7:1, b8:1, b9:1, b10:1, b11:1, b12:1;
}DS2_Report;

extern USB_ClassInfo_HID_Device_t DS2_HID_Interface;

#ifdef __cplusplus
}
#endif

#endif
