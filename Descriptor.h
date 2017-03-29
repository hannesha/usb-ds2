#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <LUFA/Drivers/USB/USB.h>

typedef struct {
	USB_Descriptor_Configuration_Header_t	Config;
	USB_Descriptor_Interface_t				HID_Interface;
	USB_HID_Descriptor_HID_t				HID_DS2HID;
	USB_Descriptor_Endpoint_t				HID_ReportINEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t {
	INTERFACE_ID_Joystick = 0,
};

enum StringDescriptors_t {
	STRING_ID_Language		= 0,
	STRING_ID_Manufacturer	= 1,
	STRING_ID_Product		= 2,
};

#define JOYSTICK_EPADDR		(ENDPOINT_DIR_IN | 1)

// 6 byte of output data
#define JOYSTICK_EPSIZE		6

#ifdef __cplusplus
}
#endif

#endif
