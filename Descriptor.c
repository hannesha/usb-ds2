#include "Descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

const USB_Descriptor_HIDReport_Datatype_t PROGMEM DS2Report[] = {
	HID_RI_USAGE_PAGE(8, 0x01), //Generic Desktop
	HID_RI_USAGE(8, 0x05), //Gamepad
	HID_RI_COLLECTION(8, 0x01), //Application
		HID_RI_USAGE(8, 0x01), //Pointer
		HID_RI_COLLECTION(8, 0x00), //Physical
			HID_RI_USAGE(8, 0x33), //Usage Rx
			HID_RI_USAGE(8, 0x34), //Usage Ry
			HID_RI_USAGE(8, 0x30), //Usage X
			HID_RI_USAGE(8, 0x31), //Usage Y
			HID_RI_LOGICAL_MINIMUM(8, 0x00),
			HID_RI_LOGICAL_MAXIMUM(8, 0xFF),
			HID_RI_REPORT_COUNT(8, 0x04),
			HID_RI_REPORT_SIZE(8, 0x08),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
		HID_RI_END_COLLECTION(0),
			
		HID_RI_USAGE(8, 0x39), //Usage Hat Switch
		HID_RI_LOGICAL_MINIMUM(8, 0x01),
		HID_RI_LOGICAL_MAXIMUM(8, 0x08),
		HID_RI_PHYSICAL_MINIMUM(8, 0x00), //0 degrees
		HID_RI_PHYSICAL_MAXIMUM(8, 270), //315 degrees
		HID_RI_UNIT(8, 0x14), //degrees
		HID_RI_UNIT_EXPONENT(8, 0x00), //no exponent
		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x04),
		HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NULLSTATE), //Absolute input with nullstate
		
		HID_RI_USAGE_PAGE(8, 0x09), //Usage Buttons
		HID_RI_USAGE_MINIMUM(8, 0x01), //Button1
		HID_RI_USAGE_MAXIMUM(8, 0x0C), //Button12
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x0C),
		HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
	HID_RI_END_COLLECTION(0),
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
	.Header				= {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification	= VERSION_BCD(1,1,0),
	.Class				= USB_CSCP_NoDeviceClass,
	.SubClass			= USB_CSCP_NoDeviceSubclass,
	.Protocol			= USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size		= FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID			= 0x03EB,
	.ProductID			= 0x2043,
	.ReleaseNumber		= VERSION_BCD(0,0,1),

	.ManufacturerStrIndex = STRING_ID_Manufacturer,
	.ProductStrIndex	= STRING_ID_Product,
	.SerialNumStrIndex	= NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config = {	
		.Header = { 
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration
		},

		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 1,
		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,
		.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)	},

	.HID_Interface = {
		.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
		.InterfaceNumber        = INTERFACE_ID_Joystick,
		.AlternateSetting       = 0x00,
		.TotalEndpoints         = 1,

		.Class                  = HID_CSCP_HIDClass,
		.SubClass               = HID_CSCP_NonBootSubclass,
		.Protocol               = HID_CSCP_NonBootProtocol,

		.InterfaceStrIndex      = NO_DESCRIPTOR
	},

	.HID_DS2HID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
		},

		.HIDSpec                = VERSION_BCD(1,1,1),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(DS2Report)
	},

	.HID_ReportINEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress        = JOYSTICK_EPADDR,
		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = JOYSTICK_EPSIZE,
		.PollingIntervalMS      = 0x05
	}
};


const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Hannes H");

const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"PSX DUALSHOCK");


uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
		const uint16_t wIndex,
		const void** const DescriptorAddress){

	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType){
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber){
				case STRING_ID_Language:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
		case HID_DTYPE_HID:
			Address = &ConfigurationDescriptor.HID_DS2HID;
			Size    = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case HID_DTYPE_Report:
			Address = &DS2Report;
			Size    = sizeof(DS2Report);
			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

#ifdef __cplusplus
}
#endif
