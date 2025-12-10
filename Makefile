# Athens Metro Manager - Makefile
# Uses local SGG library from parent directory

CXX = g++
SGG_DIR = ../sgg

CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I$(SGG_DIR)
LDFLAGS = -L$(SGG_DIR)/lib -lsgg -lGLEW -lSDL2 -lSDL2_mixer -lfreetype -framework OpenGL

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
