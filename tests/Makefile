# Input the serial port using an environment variable.
SELECTED_SERIAL_PORT=$(if $(SERIAL_PORT), $(SERIAL_PORT),notset)
# Optionally input the project PROJECT=Firmware/SIXpackDebug
SELECTED_PROJECT:=$(if $(PROJECT), $(PROJECT), .)

GEN:=../gen/test
LIB:=../libraries

FQBN=esp32:esp32:esp32cam

default: test

test: init build upload monitor

init:
	@mkdir -p $(GEN)

build:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli compile --build-path $(GEN) --fqbn $(FQBN) \
			--libraries $(LIB) \
			--libraries .

upload:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make upload"
else
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli upload --build-path $(GEN) --port $(SELECTED_SERIAL_PORT) --fqbn $(FQBN) .
endif

monitor:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make monitor"
else
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli monitor --port $(SELECTED_SERIAL_PORT)
endif
 
clean:
	@cd $(SELECTED_PROJECT) \
		&& rm -rf $(GEN)/*

