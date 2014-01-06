Configure beaglebone black to test the GPIOs
---------------------------------------------
This is a complete GPIO test system for the Autonerf project.

The main.c file consists of a interpreter to test the shoot,
reload, pan and tilt I/O's. This also includes buttons and
indicator LEDs.

Getting started
---------------------------------------------
Before the I/O's can be controled you need to set some settings
in the linux (Angstörm) system.

If the BBB is connected to your PC open the drive on your PC.
Replace uEnv.txt with the uEnv.txt in the settings_bbb folder.

WANRNING: by replacing uEnv.txt you disable the HDMI interface
on the BBB! We need those pins for the Autonerf cape.
It also starts PWM on specific pins.

Then, open a FTP or other file system connection to your BBB
and replace the .profile file in your home (~/) directory.

Copy the overlay folder to a directory of your choice. Then 
open a therminal to the BBB and login as root. Go to the 
overlay folder and run the build script. Then copy the created 
DM-GPIO-Test-0A00.dtbo file to the lib/firmware folder.
(cp DM-GPIO-Test-0A00.dtbo /lib/firmware)

Now shutdown and restart your BBB.

If everything went well the folowing envoirement variables
has to be created:
$SLOTS = /sys/devices/bone_capemgr.8/slots

$PINS = /sys/kernel/debug/pinctrl/44e10800.pinmux/pins

check this with the folowing commands:
echo $SLOTS and echo $PINS.
If this is the case try the folowing command:
cat $PINS|grep 8c0
This should give the folowing result:
0x00000007
instead of
0x00000037
This means that the pin is set to GPIO output with a pull-down
resistor.
You can test this for the other pin adresses as well, check
for the adresses the DM-GPIO-Test.dts file.

Now build the GPIO test script with the build command in the
software directory. This builds the files in the /src folder.
Notice that the c-files has to be in the src folder and header
files in the /header folder.

Now run the created executable.

Troubleshooting
----------------------------------------------
No PWM
-------
If the PWM does not work it can be that the folder is off.
This has to do with a random number generated at the end of
the PWM folder. In the pwm.c file this can be adjusted. There
is a array with the name pinSufP8 and pinSufP9. The entrys 
underneath pinValP8 22 and 80 has to be adjusted to the proper
suffix number.

To check for the suffix number go in the therminal to
/sys/devices/ocp.2/ folder. Type ls and search for the folder
called pwm_test_P8_19.xx and pwm_test_P8_36.xx. The suffix number
is the number on the xx places.

Re-compile and try whether the settings are correct.