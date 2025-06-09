# 🚀 C++ Task Manager - STL Sequential Containers Demo

A comprehensive task management application demonstrating the practical use of **STL Sequential Containers** (`std::vector` and `std::deque`) with both console and web interfaces.

## 📚 Educational Focus: STL Sequential Containers

This project showcases the practical implementation and differences between STL sequential containers:

### 🔧 Container Usage

#### `std::vector<Task>` - Regular Tasks
- **Purpose**: Store regular tasks in a dynamic array
- **Benefits**: 
  - Fast random access O(1)
  - Efficient insertion at the end O(1) amortized
  - Cache-friendly memory layout
  - Default choice for sequences
- **Use Case**: Perfect for managing a list of tasks where you frequently add new tasks and need efficient iteration

#### `std::deque<Task>` - Urgent Tasks  
- **Purpose**: Store urgent tasks requiring priority handling
- **Benefits**:
  - Fast insertion/deletion at both ends O(1)
  - Efficient for queue operations
  - Good for priority task management
- **Use Case**: Ideal for urgent tasks that need to be added to the front (highest priority) or back (standard priority) of the queue

## 🌟 Features

### Core STL Operations Demonstrated
- ✅ **Dynamic insertion** using `push_back()`, `push_front()`
- ✅ **Efficient removal** using `erase()` with iterators
- ✅ **STL algorithms** like `std::sort()`, `std::find_if()`
- ✅ **Lambda expressions** for custom sorting and searching
- ✅ **Range-based loops** for modern C++ iteration
- ✅ **Container manipulation** showing practical STL usage

### Application Features
- 📝 Add tasks (regular or urgent)
- ✅ Mark tasks as completed
- 🗑️ Delete tasks
- 🔄 Sort tasks by priority, due date, or title
- 🎯 Separate urgent and regular task management
- 🌐 Modern web interface using htmx
- 💻 Console interface for command-line usage

## 🏗️ Project Structure

```
task-management/
├── Task.h              # Task class definition with JSON serialization
├── Task.cpp            # Task class implementation
├── TaskManager.h       # Main container management (vector & deque)
├── TaskManager.cpp     # TaskManager implementation
├── main.cpp            # Console application entry point
├── web_server.cpp      # HTTP server with htmx frontend
├── Makefile           # Build configuration
└── README.md          # This file
```

## 🚀 Quick Start

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Make (optional, for using Makefile)

### Build and Run

#### Build the project:
```bash
make all
```

#### Run the web version:
```bash
make run-web
```
Then open your browser to: `http://localhost:8080`

#### Run the console version:
```bash
make run-console
```

## 🎯 STL Concepts Demonstrated

### 1. Container Selection
```cpp
class TaskManager {
private:
    std::vector<Task> tasks;      // Dynamic array for regular tasks
    std::deque<Task> urgentTasks; // Double-ended queue for urgent tasks
};
```

### 2. STL Algorithms
```cpp
// Sorting with custom comparator
std::sort(tasks.begin(), tasks.end(),
    [](const Task &a, const Task &b) { 
        return a.getPriority() > b.getPriority(); 
    });

// Finding with predicate
auto it = std::find_if(tasks.begin(), tasks.end(),
    [&title](const Task &task) { 
        return task.getTitle() == title; 
    });
```

### 3. Modern C++ Features
- Range-based loops
- Lambda expressions for sorting/filtering
- Auto type deduction
- Smart container operations

## 🌐 Web Interface Features

The htmx frontend provides:
- **Real-time updates** without page refreshes
- **Dynamic task management** with instant feedback
- **Container visualization** showing which STL container stores each task
- **Responsive design** that works on all devices

---

**Happy Learning! 🎓** 

Explore the power of STL containers and see how `std::vector` and `std::deque` can solve real-world problems efficiently! 