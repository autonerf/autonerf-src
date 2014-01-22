# Project configuration
BUILD 			= build
SOURCE 			= src
TEST 			= test
INCLUDE 		= include
EXECUTABLE		= run
TEST_EXECUTABLE = run_test

# Compiler settings
CC				= gcc
C_FLAGS			= -Wall -Werror -Wextra -O2
LD_FLAGS		= -lopencv_core -lopencv_highgui
INC_FLAGS		= -I$(INCLUDE) -I/usr/local/include/opencv

# Compiler test settings
TEST_CC 		= gcc
TEST_C_FLAGS 	= -Wall -Werror -Wextra -O0 -ggdb -DTEST=
TEST_LD_FLAGS	=
TEST_INC_FLAGS	= -I$(INCLUDE) -Itest

DTC 		= dtc
DTC_FLAGS	= -b 0

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILD)/main.o $(BUILD)/camera.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) -o $(EXECUTABLE) \
		$(BUILD)/main.o
		$(BUILD)/camera.o

$(BUILD)/main.o: main.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/main.o main.c

$(BUILD)/camera.o: $(SOURCE)/camera.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/camera.o $(SOURCE)/camera.c

$(BUILD)/filter.o: $(SOURCE)/filter.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/filter.o $(SOURCE)/filter.c

# Normal build targets
$(BUILD)/DM-GPIO-Test-00A0.dtbo: resources/DM-GPIO-Test.dts
	$(DTC) -O dtb -o $(BUILD)/DM-GPIO-Test-00A0.dtbo $(DTC_FLAGS) -@ resources/DM-GPIO-Test.dts

install: $(EXECUTABLE) $(BUILD)/DM-GPIO-Test-00A0.dtbo
	cp $(BUILD)/DM-GPIO-Test-00A0.dtbo /lib/firmware
	cp resources/.profile ~/.profile
	cp resources/uEnv.txt /media/BEAGLEBONE/uEnv.txt

uninstall:
	rm -rf /lib/firmware/DM-GPIO-Test-00A0.dtbo

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BUILD)/*.o
	rm -rf $(BUILD)/*.dtbo
	rm -rf $(TEST_EXECUTABLE)
	rm -rf ./*.png
	rm -rf ./*.ppm
