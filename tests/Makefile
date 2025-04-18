CXX ?= gcc
# path #
ROOT = ..
SRC_PATH = ../libraries
TEST_PATH = .
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable # 
BIN_NAME = run_tests

# extensions #
SRC_EXT = cpp

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name "*.$(SRC_EXT)" | sort -k 1nr | cut -f2-)
TEST_SOURCES = $(shell find $(TEST_PATH) -name "*.$(SRC_EXT)" | sort -k 1nr | cut -f2-)
# convert cpp file nanme sinto o file names
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o) $(TEST_SOURCES:$(TEST_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o) 

# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# # flags #
# COMPILE_FLAGS = -std=c++11 -Wall -Wextra -g -Wc++11-extensions

INCLUDES = -I include/ \
	   -I /usr/local/include \
	   -I $(ROOT)/libraries/robot
# Space-separated pkg-config libraries used by this project
LIBS =

debug:
	@echo SOURCE=$(SOURCES)
	@echo TEST_SOURCE=$(TEST_SOURCES)
	@echo OBJECTS=$(OBJECTS)
	@echo DEPS=$(DEP)
	@echo INCLUDES=$(INCLUDES)

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "BIN $(BIN_NAME)"

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) -o $@ ${LIBS}

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

test: all
	$(BIN_PATH)/$(BIN_NAME)
