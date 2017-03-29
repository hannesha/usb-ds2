MCU		=	atmega32u4
F_CPU	=	16000000
F_USB	=	$(F_CPU)
ARCH	=	AVR8
FILES	=	USBds2
CFILES	=	Descriptor Cpp_Glue
OBJS	=	$(FILES:=.o)
COBJS	=	$(CFILES:=.o)
MAIN	=	USBds2

#path to lufa library
LUFA_PATH				= 	lufa/
LUFA_CONFIG				=	Config.h
LUFA_CLASS				=	$(addprefix $(LUFA_PATH), LUFA/Drivers/USB/Class/)
LUFA_DEVICE				=	$(addprefix $(LUFA_CLASS), Device/)
LUFA_HID_DEVICE_FILES	=	HIDClassDevice
LUFA_HID_DEVICE_OBJS	=	$(LUFA_HID_DEVICE_FILES:=.o)
LUFA_CLASS_COMMON		=	$(addprefix $(LUFA_CLASS), Common/)
#LUFA_HID_COMMON_FILES	=	HIDParser
#LUFA_HID_COMMON_OBJS	=	$(LUFA_HID_COMMON_FILES:=.o)

LUFA_CORE				=	$(addprefix $(LUFA_PATH), LUFA/Drivers/USB/Core/)
LUFA_CORE_ARCH			=	$(addprefix $(LUFA_CORE), $(ARCH)/)
LUFA_CORE_FILES			=	ConfigDescriptors DeviceStandardReq Events HostStandardReq USBTask
LUFA_CORE_ARCH_FILES	=	$(addsuffix _$(ARCH), Device Endpoint EndpointStream Host Pipe PipeStream USBController USBInterrupt)
LUFA_CORE_OBJS			=	$(LUFA_CORE_FILES:=.o)
LUFA_CORE_ARCH_OBJS		=	$(LUFA_CORE_ARCH_FILES:=.o)
LUFA_OBJS				=	$(LUFA_CORE_OBJS) $(LUFA_CORE_ARCH_OBJS) $(LUFA_HID_DEVICE_OBJS)

CARGS	=	-Wall -Os -DF_CPU=$(F_CPU) -DF_USB=$(F_USB) -I$(LUFA_PATH) -include $(LUFA_CONFIG)
TTY		=	ttyACM0


$(OBJS):%.o: %.cpp
	avr-g++ $(CARGS) -std=c++11 -mmcu=$(MCU) -c $<

$(COBJS):%.o: %.c
	avr-gcc $(CARGS) -std=c11 -mmcu=$(MCU) -c $<

$(LUFA_CORE_OBJS):%.o: $(addprefix $(LUFA_CORE), %.c)
	avr-g++ $(CARGS) -fpermissive -mmcu=$(MCU) -c $<

$(LUFA_CORE_ARCH_OBJS):%.o: $(addprefix $(LUFA_CORE_ARCH), %.c)
	avr-g++ $(CARGS) -fpermissive -mmcu=$(MCU) -c $<

$(LUFA_HID_DEVICE_OBJS):%.o: $(addprefix $(LUFA_DEVICE), %.c)
	avr-g++ $(CARGS) -fpermissive -mmcu=$(MCU) -c $<

$(MAIN).elf: $(LUFA_OBJS) $(OBJS) $(COBJS)
	avr-g++ -mmcu=$(MCU) -o $(FILE).elf $(OBJS) $(COBJS) $(LUFA_OBJS)

$(MAIN).hex: $(MAIN).elf
	avr-objcopy -O ihex -R .eeprom $(FILE).elf $(FILE).hex

upload: $(MAIN).hex
	avrdude -p$(MCU) -P/dev/$(TTY) -cavr109 -b57600 -Uflash:w:$(FILE).hex

clean:
	rm -f $(LUFA_OBJS) $(OBJS) $(COBJS) $(MAIN).elf $(MAIN).hex

verify: $(OBJS) $(COBJS)

link: $(MAIN).elf

all: $(MAIN).hex

.PHONY: upload clean verify link all
