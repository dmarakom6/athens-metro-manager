# Athens Metro Manager - Makefile
# Uses local SGG library from parent directory

CXX = g++
SGG_DIR = ../sgg

CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I$(SGG_DIR)

# Detect Operating System
UNAME_S := $(shell uname -s)

# Common libraries
LIBS = -L$(SGG_DIR)/lib -lsgg -lSDL2 -lSDL2_mixer -lfreetype -ljsoncpp

# Include paths
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I$(SGG_DIR) -I/usr/include/jsoncpp

# OS-Specific Flags
ifeq ($(UNAME_S), Linux)
	LDFLAGS = $(LIBS) -lGL -lGLEW
endif
ifeq ($(UNAME_S), Darwin)
	CXXFLAGS += -arch x86_64
	LDFLAGS = -arch x86_64 $(LIBS) -lGLEW -framework OpenGL -framework Cocoa
endif

# Source files
SOURCES = main.cpp
HEADERS = util/GlobalState.h util/VisualAsset.h util/Station.h

# Output executable
TARGET = athens-metro-manager

# Build target
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

# Run the demo
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Rebuild
rebuild: clean $(TARGET)

.PHONY: run clean rebuild
