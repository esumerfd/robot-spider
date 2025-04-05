CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
SOURCE_PATH = . libraries
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin
SOURCES = $(foreach dir, $(SOURCE_PATH), $(wildcard $(dir)/*.cpp))
OBJECTS = $(patsubst %.cpp, $(BUILD_PATH)/%.o, $(notdir $(SOURCES)))
EXECUTABLE = $(BIN_PATH)/test_executable

debug:
	@echo SOURCE $(SOURCES)
	@echo OBJECTS $(OBJECTS)

# Rules
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BIN_PATH)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	mkdir -p $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_PATH)
