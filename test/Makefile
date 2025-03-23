default: test

compile:
	avr-gcc -v -std=c++11 -c -g \
		-I ../libraries/robot \
		-I ../libraries/logging \
		-I ../libraries/flash \
		*.cpp \
	# && avr-gcc -v -std=c++11 -c -g \
	# 	-I ../libraries/robot \
	# 	-I ../libraries/logging \
	# 	-I ../libraries/flash \
	# 	../libraries/robot/*.cpp

link:
	avr-gcc -v \
		-o ../gen/robot-spider-test ./main.o ./mover_test.o ../gen/libraries/Wire/Wire.cpp.o ../gen/libraries/Adafruit_PWM_Servo_Driver_Library/Adafruit_PWMServoDriver.cpp.o ../gen/libraries/robot/servo.cpp.o ../gen/libraries/robot/mover.cpp.o ../gen/libraries/robot/body.cpp.o ../gen/libraries/robot/robot.cpp.o ../gen/libraries/robot/board.cpp.o ../gen/libraries/Adafruit_BusIO/Adafruit_BusIO_Register.cpp.o ../gen/libraries/Adafruit_BusIO/Adafruit_GenericDevice.cpp.o ../gen/libraries/Adafruit_BusIO/Adafruit_I2CDevice.cpp.o ../gen/libraries/Adafruit_BusIO/Adafruit_SPIDevice.cpp.o ../gen/libraries/flash/flasher.cpp.o ../gen/libraries/SPI/SPI.cpp.o ../gen/libraries/logging/logging.cpp.o

run:
	../gen/robot-spider-test

test: compile link run

