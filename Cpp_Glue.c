#include "Cpp_Glue.h"

uint8_t PrevDS2HIDReport[sizeof(DS2_HID_Interface)];

USB_ClassInfo_HID_Device_t DS2_HID_Interface = {
	.Config = {
		.InterfaceNumber = INTERFACE_ID_Joystick,
		.ReportINEndpoint = {
			.Address = JOYSTICK_EPADDR,
			.Size = JOYSTICK_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = PrevDS2HIDReport,
		.PrevReportINBufferSize = sizeof(DS2_HID_Interface),
	}
};
