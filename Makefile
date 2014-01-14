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
LD_FLAGS		= -lv4l2 -lrt
INC_FLAGS		= -I$(INCLUDE)

# Compiler test settings
TEST_CC 		= gcc
TEST_C_FLAGS 	= -Wall -Werror -Wextra -O0 -ggdb -DTEST=
TEST_LD_FLAGS	=
TEST_INC_FLAGS	= -I$(INCLUDE) -Itest

DTC 		= dtc
DTC_FLAGS	= -b 0

all: $(EXECUTABLE) $(BUILD)/DM-GPIO-Test-00A0.dtbo

$(EXECUTABLE): $(BUILD)/main.o $(BUILD)/camera.o $(BUILD)/launcher.o $(BUILD)/control.o $(BUILD)/gpio.o $(BUILD)/pwm.o $(BUILD)/servo.o $(BUILD)/filter.o
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -o $(EXECUTABLE) \
		$(BUILD)/main.o \
		$(BUILD)/camera.o \
		$(BUILD)/launcher.o \
		$(BUILD)/control.o \
		$(BUILD)/filter.o \
		$(BUILD)/servo.o \
		$(BUILD)/gpio.o \
		$(BUILD)/pwm.o

test: $(BUILD)/test.debugger.o $(BUILD)/test.main.o $(BUILD)/test.camera.o $(BUILD)/test.filter.o
	$(CC) $(TEST_C_FLAGS) $(TEST_LD_FLAGS) $(TEST_INC_FLAGS) -o $(TEST_EXECUTABLE) \
		$(BUILD)/test.debugger.o \
		$(BUILD)/test.main.o \
		$(BUILD)/test.camera.o \
		$(BUILD)/test.filter.o \
		$(BUILD)/test.vision.o

$(BUILD)/main.o: main.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/main.o main.c

# Normal build targets
$(BUILD)/camera.o: $(SOURCE)/camera.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/camera.o $(SOURCE)/camera.c

$(BUILD)/launcher.o: $(SOURCE)/launcher.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/launcher.o $(SOURCE)/launcher.c

$(BUILD)/control.o: $(SOURCE)/io/control.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/control.o $(SOURCE)/io/control.c

$(BUILD)/filter.o: $(SOURCE)/filter.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/filter.o $(SOURCE)/filter.c

$(BUILD)/servo.o: $(SOURCE)/io/servo.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/servo.o $(SOURCE)/io/servo.c

$(BUILD)/gpio.o: $(SOURCE)/io/gpio.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/gpio.o $(SOURCE)/io/gpio.c

$(BUILD)/pwm.o: $(SOURCE)/io/pwm.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/pwm.o $(SOURCE)/io/pwm.c

$(BUILD)/DM-GPIO-Test-00A0.dtbo: resources/DM-GPIO-Test.dts
	$(DTC) -O dtb -o $(BUILD)/DM-GPIO-Test-00A0.dtbo $(DTC_FLAGS) -@ resources/DM-GPIO-Test.dts

# Test build targets
$(BUILD)/test.main.o: main.c
	$(CC) $(TEST_C_FLAGS) $(TEST_INC_FLAGS) -c -o $(BUILD)/test.main.o main.c

$(BUILD)/test.debugger.o: $(TEST)/debugger.c
	$(CC) $(TEST_C_FLAGS) $(TEST_INC_FLAGS) -c -o $(BUILD)/test.debugger.o $(TEST)/debugger.c

$(BUILD)/test.camera.o: $(TEST)/camera.c
	$(CC) $(TEST_C_FLAGS) $(TEST_INC_FLAGS) -c -o $(BUILD)/test.camera.o $(TEST)/camera.c

$(BUILD)/test.filter.o: $(SOURCE)/filter.c
	$(CC) $(TEST_C_FLAGS) $(TEST_INC_FLAGS) -c -o $(BUILD)/test.filter.o $(SOURCE)/filter.c

$(BUILD)/test.vision.o: $(SOURCE)/vision.c
	$(CC) $(TEST_C_FLAGS) $(TEST_INC_FLAGS) -c -o $(BUILD)/test.vision.o $(SOURCE)/vision.c

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
	rm -rf $(TEST_EXECUTABLE)
	rm -rf ./*.png
	rm -rf ./*.ppm
