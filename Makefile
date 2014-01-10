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

$(EXECUTABLE): $(BUILD)/main.o $(BUILD)/camera.o $(BUILD)/launcher.o $(BUILD)/control.o $(BUILD)/gpio.o $(BUILD)/pwm.o $(BUILD)/servo.o
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -o $(EXECUTABLE) \
		$(BUILD)/main.o \
		$(BUILD)/camera.o \
		$(BUILD)/launcher.o \
		$(BUILD)/control.o \
		$(BUILD)/servo.o \
		$(BUILD)/gpio.o \
		$(BUILD)/pwm.o

$(BUILD)/main.o: main.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/main.o main.c

$(BUILD)/camera.o: $(SOURCE)/camera.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/camera.o $(SOURCE)/camera.c

$(BUILD)/launcher.o: $(SOURCE)/launcher.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/launcher.o $(SOURCE)/launcher.c

$(BUILD)/control.o: $(SOURCE)/io/control.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/control.o $(SOURCE)/io/control.o

$(BUILD)/gpio.o: $(SOURCE)/io/gpio.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/gpio.o $(SOURCE)/io/gpio.o

$(BUILD)/pwm.o: $(SOURCE)/io/pwm.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/pwm.o $(SOURCE)/io/pwm.o

$(BUILD)/servo.o: $(SOURCE)/io/servo.c
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/servo.o $(SOURCE)/io/servo.o

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
