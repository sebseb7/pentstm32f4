LIB = libSTM32F1xx_drivers.a

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -O0 -mthumb
CFLAGS += -mcpu=cortex-m3  -ffreestanding
CFLAGS += -DSTM32F10X_CL=1 
#CFLAGS += -mcpu=cortex-m4  -ffreestanding -nostdlib -Wl,--gc-sections -fsingle-precision-constant -funsigned-char -Wundef -Wsign-compare -Wunreachable-code -Wstrict-prototypes
CFLAGS += -I../inc -I../../core -DARM_MATH_CM3 -DUSE_STDPERIPH_DRIVER

SRCS = $(wildcard ../src/*.c)

OBJS = $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	@$(AR) -r $(LIB) $(OBJS)
	@$(RANLIB) $(LIB)

%.o : %.c
	@echo "  GCC $^"
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
