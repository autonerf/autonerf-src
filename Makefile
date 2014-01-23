# Project configuration
BUILD 			= build
SOURCE 			= src
TEST 			= test
INCLUDE 		= include
EXECUTABLE		= run
TEST_EXECUTABLE = run_test

# Compiler settings
CC				= gcc
C_FLAGS			= -O2 -ggdb -std=c99
LD_FLAGS		= -lopencv_core -lopencv_highgui -lopencv_imgproc
INC_FLAGS		= -I$(INCLUDE) -I/usr/local/include/opencv -I/usr/include/opencv -Itest

DTC 		= dtc
DTC_FLAGS	= -b 0

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILD)/main.o \
		$(BUILD)/camera.o \
		$(BUILD)/filter.o \
		$(BUILD)/debugger.o \
		$(BUILD)/vision.o \
		$(BUILD)/launcher.o \
		$(BUILD)/io.control.o \
		$(BUILD)/io.gpio.o \
		$(BUILD)/io.pwm.o \
		$(BUILD)/io.servo.o
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -o $(EXECUTABLE) \
		$(BUILD)/main.o \
		$(BUILD)/camera.o \
		$(BUILD)/filter.o \
		$(BUILD)/debugger.o \
		$(BUILD)/vision.o \
		$(BUILD)/launcher.o \
		$(BUILD)/io.control.o \
		$(BUILD)/io.gpio.o \
		$(BUILD)/io.pwm.o \
		$(BUILD)/io.servo.o

$(BUILD)/main.o: main.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/main.o main.c

$(BUILD)/camera.o: $(SOURCE)/camera.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/camera.o $(SOURCE)/camera.c

$(BUILD)/filter.o: $(SOURCE)/filter.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/filter.o $(SOURCE)/filter.c

$(BUILD)/debugger.o: test/debugger.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/debugger.o test/debugger.c

$(BUILD)/vision.o: $(SOURCE)/vision.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/vision.o $(SOURCE)/vision.c

$(BUILD)/launcher.o: $(SOURCE)/launcher.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/launcher.o $(SOURCE)/launcher.c

$(BUILD)/io.control.o: $(SOURCE)/io/control.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/io.control.o $(SOURCE)/io/control.c

$(BUILD)/io.gpio.o: $(SOURCE)/io/gpio.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/io.gpio.o $(SOURCE)/io/gpio.c

$(BUILD)/io.pwm.o: $(SOURCE)/io/pwm.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/io.pwm.o $(SOURCE)/io/pwm.c

$(BUILD)/io.servo.o: $(SOURCE)/io/servo.c
	$(CC) $(C_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/io.servo.o $(SOURCE)/io/servo.c

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
