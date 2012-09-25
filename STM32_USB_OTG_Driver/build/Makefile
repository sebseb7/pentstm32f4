LIB = libSTM32_USB_OTG_Driver.a

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -O2 -mlittle-endian -mthumb
CFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffreestanding -nostdlib -Wl,--gc-sections -fsingle-precision-constant -funsigned-char -Wundef -Wsign-compare -Wunreachable-code -Wstrict-prototypes
CFLAGS += -I../../usb -I../../STM32F4_drivers/inc -I../inc -I../../core -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS=1

SRCS = \
../src/usb_core.c \
../src/usb_dcd.c \
../src/usb_dcd_int.c \

OBJS = $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) -r $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
