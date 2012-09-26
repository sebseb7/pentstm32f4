PROJECT=template



LSCRIPT=core/stm32_flash.ld

OPTIMIZATION = -O1

#########################################################################

SRC=$(wildcard core/*.c usb/*.c *.c) \
	STM32_USB_Device_Library/Core/src/usbd_core.c \
	STM32_USB_Device_Library/Core/src/usbd_req.c \
	STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
	STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c \
	STM32_USB_OTG_Driver/src/usb_core.c \
	STM32_USB_OTG_Driver/src/usb_dcd.c \
	STM32_USB_OTG_Driver/src/usb_dcd_int.c 
ASRC=$(wildcard core/*.s)
OBJECTS= $(SRC:.c=.o) $(ASRC:.s=.o)
LSTFILES= $(SRC:.c=.lst)
HEADERS=$(wildcard core/*.h usb/*.h *.h)

#  Compiler Options
GCFLAGS = -DUSE_USB_OTG_FS=1 -ffreestanding -std=gnu99 -mcpu=cortex-m4 -mthumb $(OPTIMIZATION) -I. -Icore -Iusb -Wl,-gc-sections -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER -nostdlib
GCFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
GCFLAGS+=-ISTM32_USB_Device_Library/Class/cdc/inc
GCFLAGS+=-ISTM32_USB_OTG_Driver/inc
GCFLAGS+=-ISTM32_USB_Device_Library/Core/inc
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fno-common -fdata-sections 
# Debug stuff
GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F4_drivers/inc


LDFLAGS = -mcpu=cortex-m4 -mthumb $(OPTIMIZATION) -nostartfiles -gc-sections -T$(LSCRIPT) 
LDFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
LDFLAGS+= -LSTM32F4_drivers/build -lSTM32F4xx_drivers -lm
#LDFLAGS+= -LSTM32_USB_OTG_Driver/build -lSTM32_USB_OTG_Driver


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

#all: STM32_USB_OTG_Driver/build/libSTM32_USB_OTG_Driver.a STM32F4_drivers/build/libSTM32F4_drivers.a $(PROJECT).bin Makefile stats
all: STM32F4_drivers/build/libSTM32F4_drivers.a $(PROJECT).bin Makefile stats
#	arm-none-eabi-objdump -d $(PROJECT).elf > out.dump

STM32F4_drivers/build/libSTM32F4_drivers.a:
	make -C STM32F4_drivers/build

STM32_USB_OTG_Driver/build/libSTM32_USB_OTG_Driver.a:
	make -C STM32_USB_OTG_Driver/build

$(PROJECT).bin: $(PROJECT).elf Makefile
	$(OBJCOPY) -R .stack -O binary $(PROJECT).elf $(PROJECT).bin

$(PROJECT).elf: $(OBJECTS) Makefile
	$(GCC) $(OBJECTS) $(LDFLAGS)  -o $(PROJECT).elf

stats: $(PROJECT).elf Makefile
	$(SIZE) $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) $(PROJECT).bin
	$(REMOVE) $(PROJECT).elf
	make -C STM32F4_drivers/build clean
	make -C STM32_USB_OTG_Driver/build clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	$(GCC) $(GCFLAGS) -o $@ -c $<

%.o: %.s Makefile 
	$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: all
	./dfu-util -a 0 -s 0x08000000 -D $(PROJECT).bin -R
