# tools

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

# this is ST's free STM32CubeProgrammer package. if you installed it under sudo then it should be in the location below

PROGRAMMER = /usr/local/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer.sh
             
# flags for tools

INCLUDE = -IUSB_DEVICE/App -IUSB_DEVICE/Target -ICore/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -IMiddlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc -IMiddlewares/ST/STM32_Audio/Addons/SVC/Inc -IMiddlewares/ST/STM32_Audio/Addons/GREQ/Inc -IMiddlewares/ST/STM32_Audio/Addons/Common/Inc -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IDrivers/CMSIS/Include
CFLAGS = -mcpu=cortex-m4 -g3 -DUSE_HAL_DRIVER -DSTM32F446xx -ffunction-sections -fdata-sections -Wall -fstack-usage --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb
LDFLAGS = -mcpu=cortex-m4 --specs=nosys.specs --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--gc-sections -static -LMiddlewares/ST/STM32_Audio/Addons/SVC/Lib -LMiddlewares/ST/STM32_Audio/Addons/GREQ/Lib
ASMFLAGS = -mcpu=cortex-m4 -g3 -x assembler-with-cpp --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb

# your targets:
#   'make release' for the optimised build (same as just 'make')
#   'make debug' for a build with symbols and no optimisation
# include the 'flash' target to write to your device connected with ST-Link, e.g:
#   'make release flash'
#   'make debug flash'
# if switching between 'release' and 'debug' then do a 'make clean' first, or delete the 'build' directory.

release: CFLAGS += -O3
debug: CFLAGS += -DDEBUG -g3 -O0

release: hex bin lst size
debug: hex bin lst size

# C, C++ and assembly sources

CSRC := $(shell find . -name "*.c")
CPPSRC := $(shell find . -name "*.cpp")
ASMSRC := $(shell find . -name "*.s")

# equivalent objects for the sources

OBJ := $(CSRC:%.c=build/%.o) $(CPPSRC:%.cpp=build/%.o) $(ASMSRC:%.s=build/%.o)

# everything gets built into a 'build' sub-directory

build/%.o: %.c
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) ${INCLUDE} -c $< -o $@

build/%.o: %.cpp
	mkdir -p "$(@D)"
	$(CC) $(CFLAGS) ${INCLUDE} -c $< -o $@

build/%.o: %.s
	mkdir -p "$(@D)"
	$(CC) $(ASMFLAGS) ${INCLUDE} -c $< -o $@

# linker

elf: $(OBJ)
	$(CC) -o build/usb-microphone.elf $(OBJ) $(LDFLAGS) -lSVC -lGREQ -TSTM32F446RCTX_FLASH.ld -Wl,-Map=build/usb-microphone.map -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

# convert elf to ihex for flashing

hex: elf
	$(OBJCOPY) -O ihex build/usb-microphone.elf build/usb-microphone.hex

# binary

bin: elf
	$(OBJCOPY) -O binary build/usb-microphone.elf build/usb-microphone.bin

# assembly listing

lst: elf
	$(OBJDUMP) -h -S build/usb-microphone.elf > build/usb-microphone.lst

# size information to show MCU memory utilisation

size: elf
	$(SIZE) --format=berkeley build/usb-microphone.elf | tee build/usb-microphone.size

# programmer (flags = use SWD, connect under reset, hardware reset, program, verify, reset-and-run)

flash: elf
	$(PROGRAMMER) -c port=SWD mode=UR reset=HWrst -d build/usb-microphone.elf -v -hardRst

# clean up

clean:
	rm -rf build
