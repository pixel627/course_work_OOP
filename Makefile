CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Ithird_party
LDFLAGS = -lsqlite3

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp) \
           $(wildcard $(SRC_DIR)/core/*.cpp) \
           $(wildcard $(SRC_DIR)/models/*.cpp) \
           $(wildcard $(SRC_DIR)/ui/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE = computer_club

all: $(BUILD_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/models
	@mkdir -p $(BUILD_DIR)/ui

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) data/

.PHONY: all clean