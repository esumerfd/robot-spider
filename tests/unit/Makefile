# Input the serial port using an environment variable.
SELECTED_SERIAL_PORT=$(if $(SERIAL_PORT), $(SERIAL_PORT),notset)

GEN:=gen
LIB:=../../libraries

FQBN=esp32:esp32:esp32cam

default: build

test: build

run: init build upload monitor

init:
	@mkdir -p $(GEN)

build:
	@arduino-cli compile --build-path $(GEN) --fqbn $(FQBN) \
		--libraries $(LIB) \
		unit.ino

upload:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make upload"
else
	@arduino-cli upload --build-path $(GEN) --port $(SELECTED_SERIAL_PORT) --fqbn $(FQBN) unit.ino
endif

monitor:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make monitor"
else
	@arduino-cli monitor --port $(SELECTED_SERIAL_PORT)
endif

clean:
	@rm -rf $(GEN)/*

