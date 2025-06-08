CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -pthread

# Windows specific flags
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
    TARGET = task_manager.exe
    WEB_TARGET = web_server.exe
else
    TARGET = task_manager
    WEB_TARGET = web_server
endif

# Source files
TASK_SOURCES = main.cpp Task.cpp TaskManager.cpp
WEB_SOURCES = web_server.cpp Task.cpp

# Object files
TASK_OBJECTS = $(TASK_SOURCES:.cpp=.o)
WEB_OBJECTS = $(WEB_SOURCES:.cpp=.o)

# Default target
all: $(TARGET) $(WEB_TARGET)

# Console application
$(TARGET): $(TASK_OBJECTS)
	$(CXX) $(TASK_OBJECTS) -o $(TARGET) $(LDFLAGS)

# Web server application
$(WEB_TARGET): $(WEB_OBJECTS)
	$(CXX) $(WEB_OBJECTS) -o $(WEB_TARGET) $(LDFLAGS)

# Object file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f *.o $(TARGET) $(WEB_TARGET)
ifeq ($(OS),Windows_NT)
	del /f *.o $(TARGET) $(WEB_TARGET) 2>nul || true
endif

# Run console version
run-console: $(TARGET)
	./$(TARGET)

# Run web server
run-web: $(WEB_TARGET)
	./$(WEB_TARGET)

# Install dependencies (placeholder for future use)
install-deps:
	@echo "All dependencies are part of C++ standard library"

.PHONY: all clean run-console run-web install-deps 