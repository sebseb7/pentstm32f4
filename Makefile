PROJECT=template



LSCRIPT=core/stm32_flash.ld

OPTIMIZATION = -O1

#########################################################################

SRC=$(wildcard core/*.c *.c) 
ASRC=$(wildcard core/*.s)
OBJECTS= $(SRC:.c=.o) $(ASRC:.s=.o)
LSTFILES= $(SRC:.c=.lst)
HEADERS=$(wildcard core/*.h *.h)

#  Compiler Options
GCFLAGS = -ffreestanding -std=gnu99 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb $(OPTIMIZATION) -I. -Icore -Wl,--gc-sections -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER -nostdlib
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code  
# Optimizazions
GCFLAGS += -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fdata-sections -fno-common -fdata-sections -ffunction-sections
# Debug stuff
GCFLAGS += -Wa,-adhlns=$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F4_drivers/inc


LDFLAGS = -mcpu=cortex-m4 -mthumb $(OPTIMIZATION) -nostartfiles --gc-sections  -T$(LSCRIPT) 
LDFLAGS+= -LSTM32F4_drivers/build -lSTM32F4xx_drivers


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: STM32F4_drivers/build/libSTM32F4_drivers.a $(PROJECT).bin Makefile stats
#	arm-none-eabi-objdump -d $(PROJECT).elf > out.dump

STM32F4_drivers/build/libSTM32F4_drivers.a:
	make -C STM32F4_drivers/build

tools/stm32flash:
	make -C tools

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
	make -C tools clean

#########################################################################

%.o: %.c Makefile $(HEADERS)
	$(GCC) $(GCFLAGS) -o $@ -c $<

%.o: %.s Makefile 
	$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: all
	dfu-util -a 0 -s 0x08000000 -D $(PROJECT).bin
