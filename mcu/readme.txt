
Compiling locally
-----------------
Download the ARM gcc toolchain from here https://launchpad.net/gcc-arm-embedded
Extract to some nice folder

Download the ARM mbed library
Extract to this directory

Edit the Makefile in the following way:
	GCC_BIN = <the folder you extracted the archive to/bin>
	MBED_DRIVE = <the drive that the mbed appears on>

	...

	$(PROJECT).bin: $(PROJECT).elf
        $(OBJCOPY) -O binary $< $@ && cp $(PROJECT).bin $(MBED_DRIVE)

In the last line we copy the project binary file onto the mbed drive.
The && is a command joiner, which means the next command will only be
executed if the last one was successful.

Very useful!

Compiling online
----------------
* Go to the ARM mbed online compiler
* Paste the source files in there
* Compile

Using serial port on LINUX
--------------------------
* Install screen
* ls /dev/ttyACM*
* Scribble down whatever shows up (for me it was /dev/ttyACM0)
  This path is where the serial port is located, so to speak
* sudo screen /dev/<whatever you scribbled>

Yayifications!
