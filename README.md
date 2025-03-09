# Robot Spider

NOTE: It doesn't work yet.

Hardware based on a free Robotics hexapod by delta3robotics. 

* https://cults3d.com/de/modell-3d/verschiedene/sixpack

My goal is to re-write the software using various techniques with some experiemental goals:

* remove all code from ino file.
* use C++ instances vs global status structures.
* declaritive memory allocation.
* testing library

# Install - Mac only.

* brew install arduino-cli
* USB to Serial
    * Macs older than Maverics need these drivers, but new are good to go.
        * Install FTDI drivers: https://ftdichip.com
* make init

# Build

The build step installs the packages and runs the arduino-cli compile to generate the image

* make build

# Upload

You will have to work out the serial port to upload to. Once you have plugged it in, you should see it in this list

* make usb

Then you can upload the built solution:

* SERIAL_PORT=/dev/cu.usbserial-OR-SOMETHING make upload

# Design

Early guesses that what it might look like.

Robot has head with the camera, a body with left and right legs. Both left and right have 3 legs each and each leg has a sholder and a knee.

The sholder and knee hold the servos.

Movement is managed by encoded sequences of movers, that can be read by a sequencer to engage the machanics.

A sequence can be a hard coded set of movements of sholder/knee angles or a generated set of movements.

Lifecycle start from off, to initialized, test cycle, ready to working when an operation commences. An operation can be user selected or generated.

When an operation completes, a queue operation is check for, if none then return to ready stable state.



