PROJECT=template


STM32F=4
LSCRIPT=core/stm32f$(STM32F)xx_flash.ld

OPTIMIZATION = -O1

ifeq ($(STM32F),2)
CORTEXM=3
else
CORTEXM=4
endif

#########################################################################

SRC=$(wildcard usb/*.c *.c) \
	core/stm32f$(STM32F)xx_it.c core/system_stm32f$(STM32F)xx.c \
	STM32_USB_OTG_Driver/src/usb_core.c \
	STM32_USB_OTG_Driver/src/usb_dcd.c \
	STM32_USB_OTG_Driver/src/usb_dcd_int.c 
ASRC=core/startup_stm32f$(STM32F)xx.s
OBJECTS= $(SRC:.c=.o) $(ASRC:.s=.o)
LSTFILES= $(SRC:.c=.lst)
HEADERS=$(wildcard core/*.h usb/*.h *.h)

#  Compiler Options
GCFLAGS = -DUSE_USB_OTG_FS=1 -ffreestanding -std=gnu99 -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -I. -Icore -Iusb -Wl,-gc-sections -DARM_MATH_CM$(CORTEXM) -DUSE_STDPERIPH_DRIVER -nostdlib
ifeq ($(CORTEXM),4)
GCFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
endif
GCFLAGS+= -DDISCOVERY=1
GCFLAGS+=-ISTM32_USB_Device_Library/Class/cdc/inc
GCFLAGS+=-ISTM32_USB_OTG_Driver/inc
GCFLAGS+=-ISTM32_USB_Device_Library/Core/inc
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fno-common -fdata-sections 
# Debug stuff
GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F$(STM32F)_drivers/inc


LDFLAGS = -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -nostartfiles -gc-sections -T$(LSCRIPT) 
ifeq ($(CORTEXM),4)
LDFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
endif
LDFLAGS+= -LSTM32F$(STM32F)_drivers/build -lSTM32F$(STM32F)xx_drivers -lm
LDFLAGS+= -LSTM32_USB_Device_Library/build -lUSBCDC
#LDFLAGS+= -LSTM32_USB_OTG_Driver/build -lSTM32_USB_OTG_Driver


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: STM32F$(STM32F)_drivers/build/libSTM32F$(STM32F)_drivers.a $(PROJECT).bin Makefile 
	@$(SIZE) $(PROJECT).elf

STM32F$(STM32F)_drivers/build/libSTM32F$(STM32F)_drivers.a:
	@make -C STM32F$(STM32F)_drivers/build

STM32_USB_OTG_Driver/build/libSTM32_USB_OTG_Driver.a:
	make -C STM32_USB_OTG_Driver/build

$(PROJECT).bin: $(PROJECT).elf Makefile
	@echo "generating $(PROJECT).bin"
	@$(OBJCOPY) -R .stack -O binary $(PROJECT).elf $(PROJECT).bin

$(PROJECT).elf: $(OBJECTS) Makefile
	@echo "  LD $(PROJECT).elf"
	@$(GCC) $(OBJECTS) $(LDFLAGS)  -o $(PROJECT).elf

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) $(PROJECT).bin
	$(REMOVE) $(PROJECT).elf
	make -C STM32F$(STM32F)_drivers/build clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	@echo "  GCC $<"
	@$(GCC) $(GCFLAGS) -o $@ -c $<

%.o: %.s Makefile 
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: all
	dfu-util -a 0 -s 0x08000000 -D $(PROJECT).bin -R

.PHONY : clean all flash
