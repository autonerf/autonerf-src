# Project configuration
BUILD 		= build
SOURCE 		= src
INCLUDE 	= include
EXECUTABLE	= run

# Compiler settings
CC			= gcc
C_FLAGS		= -Wall -Werror -Wextra
LD_FLAGS	= -lv4l2 -lrt
INC_FLAGS	= -I$(INCLUDE)

DTC 		= dtc
DTC_FLAGS	= -b 0

all: $(EXECUTABLE) $(BUILD)/DM-GPIO-Test-00A0.dtbo

$(EXECUTABLE):

$(BUILD)/DM-GPIO-Test-00A0.dtbo: resources/DM-GPIO-Test.dts
	$(DTC) -O dtb -o $(BUILD)/DM-GPIO-Test-00A0.dtbo $(DTC_FLAGS) -@ resources/overlay/DM-GPIO-Test.dts

install:
	cp $(BUILD)/DM-GPIO-Test-00A0.dtbo /lib/firmware
	cp resources/.profile ~/.profile
	cp resources/uEnv.txt /media/BEAGLEBONE/uEnv.txt

uninstall:
	rm -rf /lib/firmware/DM-GPIO-Test-00A0.dtbo

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BUILD)/*.o
	rm -rf $(BUILD)/*.dtbo
