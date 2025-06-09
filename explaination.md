# C++ Task Management System - File Structure & Function Explanation

## Overview
This C++ task management system demonstrates the use of STL (Standard Template Library) sequential containers (`std::vector` and `std::deque`) to manage regular and urgent tasks. The system provides both console-based and web-based interfaces.

## File-by-File Analysis

### 1. **Task.h** (Header File)
**Purpose**: Defines the `Task` class - the fundamental data structure for representing individual tasks.

**Key Components**:
- **Private Members**:
  - `static int nextId`: Auto-incrementing ID counter for unique task identification
  - `int id`: Unique identifier for each task
  - `std::string title`: Task title
  - `std::string description`: Detailed task description
  - `bool completed`: Task completion status
  - `std::chrono::system_clock::time_point dueDate`: Due date using C++ chrono library
  - `int priority`: Priority level (1-5)

- **Public Methods**:
  - Constructor for creating new tasks
  - Getter methods for accessing private data
  - Setter methods for modifying task properties
  - `getDueDateString()`: Converts chrono time_point to readable date string
  - `toJson()`: Serializes task data to JSON format for web interface
  - `display()`: Console output formatting

**Why it's needed**: This header file is essential as it defines the core data structure that both the TaskManager and web server depend on.

### 2. **TaskManager.h** (Header File)
**Purpose**: Defines the `TaskManager` class - the main business logic layer that manages collections of tasks using STL containers.

**Key Components**:
- **Private Members**:
  - `std::vector<Task> tasks`: Stores regular tasks (efficient for sequential access and end insertions)
  - `std::deque<Task> urgentTasks`: Stores urgent tasks (efficient for both front and back insertions/deletions)

- **Public Methods**:
  - `addTask()`: Multiple overloads for adding regular tasks
  - `addUrgentTask()`: Adds tasks to deque with option for front/back insertion
  - `removeTask()`/`removeTaskById()`: Remove tasks by title or ID
  - `removeUrgentTask()`/`removeUrgentTaskById()`: Remove urgent tasks
  - `markTaskCompleted()`: Mark tasks as completed by ID or title
  - `displayAllTasks()`: Console display of all tasks
  - Sorting methods: `sortTasksByPriority()`, `sortTasksByDueDate()`, `sortTasksByTitle()`
  - `getAllTasksJson()`: Returns all tasks in JSON format
  - Utility methods for getting task counts and combined task lists

**STL Container Usage**:
- **`std::vector<Task>`**: Used for regular tasks because:
  - Fast random access O(1)
  - Efficient insertion at the end O(1) amortized
  - Memory efficient (contiguous storage)
  - Good for sorting operations

- **`std::deque<Task>`**: Used for urgent tasks because:
  - Fast insertion/deletion at both ends O(1)
  - Better for priority queue-like behavior
  - Allows flexible task prioritization

**Why it's needed**: This is the core business logic layer that provides all task management functionality. Both the console app and web server depend on this class.

### 3. **Task.cpp** (Implementation File)
**Purpose**: Contains implementation details for the Task class.

**Content**:
```cpp
#include "Task.h"
int Task::nextId = 1;  // Initialize static member
```

**Why it's needed**: 
- Initializes the static member variable `nextId`
- In C++, static member variables must be defined in a source file
- Without this file, you'd get linker errors about undefined static members

### 4. **main.cpp** (Console Application)
**Purpose**: Demonstrates the task management system through a console interface.

**Functionality**:
- Creates a `TaskManager` instance
- Demonstrates adding regular and urgent tasks
- Shows task completion, sorting, and removal operations
- Displays task statistics
- Uses a helper function `createDueDate()` for date creation

**Key Demonstrations**:
- Vector operations (adding regular tasks)
- Deque operations (adding urgent tasks to front/back)
- Task management operations (complete, sort, remove)
- STL algorithm usage (sorting)

**Why it's needed**: This file is NOT essential for the web server to work, but it's useful for:
- Testing the TaskManager functionality
- Demonstrating console-based usage
- Development and debugging

### 5. **web_server.cpp** (Web Server Implementation)
**Purpose**: Provides an HTTP web interface for the task management system using raw socket programming.

**Key Components**:
- **`SimpleHttpServer` Class**:
  - Cross-platform socket handling (Windows/Unix)
  - HTTP request parsing and response generation
  - Integration with TaskManager for web-based task operations

- **HTTP Endpoints**:
  - `GET /`: Main page with HTML interface
  - `GET /tasks`: Returns task list as HTML
  - `POST /add-task`: Adds new tasks
  - `POST /complete-task`: Marks tasks as completed
  - `POST /delete-task`: Removes tasks
  - `POST /sort-tasks`: Sorts tasks by different criteria

- **Features**:
  - Modern HTML interface with HTMX for dynamic updates
  - CSS styling for professional appearance
  - Form handling for task creation
  - Real-time task updates without page refreshes
  - Demonstrates STL container usage in web context

**Why it's needed**: This provides the web interface for the task management system. It's essential if you want web-based access to the task manager.

## File Dependencies & Requirements

### Essential Files for Web Server:
1. **`Task.h`** ✅ - Core data structure
2. **`Task.cpp`** ✅ - Static member initialization
3. **`TaskManager.h`** ✅ - Business logic layer
4. **`web_server.cpp`** ✅ - Web interface

### Optional Files:
- **`main.cpp`** ❌ - Only needed for console demonstration

### 6. **Makefile** (Build Configuration)
**Purpose**: Automates the compilation and linking process for the C++ project, handling cross-platform builds and dependencies.

**Key Components**:
- **Compiler Configuration**:
  - `CXX = g++`: Specifies the C++ compiler
  - `CXXFLAGS = -std=c++17 -Wall -Wextra -O2`: Compiler flags for C++17 standard, warnings, and optimization
  - `LDFLAGS = -pthread`: Linker flags for threading support

- **Cross-Platform Support**:
  ```makefile
  ifeq ($(OS),Windows_NT)
      LDFLAGS += -lws2_32    # Windows socket library
      TARGET = task_manager.exe
      WEB_TARGET = web_server.exe
  else
      TARGET = task_manager    # Unix/Linux executables
      WEB_TARGET = web_server
  endif
  ```

- **Source File Management**:
  - `TASK_SOURCES`: Files needed for console application (main.cpp, Task.cpp, TaskManager.cpp)
  - `WEB_SOURCES`: Files needed for web server (web_server.cpp, Task.cpp)
  - Automatically generates object file lists from source files

- **Build Targets**:
  - `all`: Builds both console and web applications
  - `$(TARGET)`: Builds the console application
  - `$(WEB_TARGET)`: Builds the web server
  - `%.o: %.cpp`: Pattern rule for compiling source files to object files

- **Utility Targets**:
  - `clean`: Removes compiled files (cross-platform compatible)
  - `run-console`: Builds and runs the console application
  - `run-web`: Builds and runs the web server
  - `install-deps`: Placeholder (no external dependencies needed)

**Why it's needed**: 
- **Automation**: Eliminates need to manually compile each file
- **Dependency Management**: Automatically recompiles only changed files
- **Cross-Platform**: Handles Windows vs Unix differences automatically
- **Convenience**: Provides easy commands like `make run-web`
- **Maintenance**: Centralizes build configuration

**Usage Examples**:
```bash
make all           # Build both applications
make run-web       # Build and run web server
make run-console   # Build and run console app
make clean         # Remove all compiled files
```

**Note**: While not strictly required (you could compile manually), the Makefile is highly recommended for:
- Efficient development workflow
- Consistent builds across different systems
- Easy project sharing and deployment

## Why Header Files (.h) are Needed

### 1. **Separation of Interface and Implementation**
- Header files contain class declarations (interface)
- Implementation files contain method definitions
- Allows multiple files to use the same class without code duplication

### 2. **Compilation Efficiency**
- Headers are included where needed
- Compiler can check interfaces without full implementation
- Enables separate compilation of different modules

### 3. **Code Organization**
- Clear separation between what a class does (header) and how it does it (implementation)
- Makes code more maintainable and readable
- Facilitates team development

### 4. **Include Guards**
Both header files use include guards (`#ifndef`, `#define`, `#endif`) to prevent multiple inclusions that would cause compilation errors.

## System Architecture

```
┌─────────────┐    ┌──────────────────┐    ┌─────────────┐
│   Task.h    │───▶│  TaskManager.h   │───▶│main.cpp     │
│   Task.cpp  │    │                  │    │web_server.cpp│
└─────────────┘    └──────────────────┘    └─────────────┘
     Core              Business Logic         Applications
   Data Model            Layer                (Console/Web)
```

## Minimum Files Needed

**For Console Application**: Task.h, Task.cpp, TaskManager.h, main.cpp
**For Web Application**: Task.h, Task.cpp, TaskManager.h, web_server.cpp
**For Both**: All 5 files

The header files are absolutely essential because they define the interfaces that other files depend on. Without them, the implementation files cannot access the class definitions they need to function.
