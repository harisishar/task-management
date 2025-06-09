<<<<<<< HEAD
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>

class Task
{
private:
    std::string title;
    std::string description;
    std::string priority;
    std::string dueDate;
    bool completed;

public:
    Task(const std::string &t, const std::string &d, const std::string &p, const std::string &dd)
        : title(t), description(d), priority(p), dueDate(dd), completed(false) {}

    // Getters
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::string getPriority() const { return priority; }
    std::string getDueDate() const { return dueDate; }
    bool isCompleted() const { return completed; }

    // Setters
    void setCompleted(bool status) { completed = status; }
    void setTitle(const std::string &t) { title = t; }
    void setDescription(const std::string &d) { description = d; }
    void setPriority(const std::string &p) { priority = p; }
    void setDueDate(const std::string &dd) { dueDate = dd; }
};

class TaskManager
{
private:
    std::vector<Task> normalTasks;  // For regular tasks
    std::deque<Task> priorityTasks; // For priority tasks that need quick front/back access

public:
    // Add a new task
    void addTask(const Task &task)
    {
        if (task.getPriority() == "High")
        {
            priorityTasks.push_front(task);
        }
        else if (task.getPriority() == "Low")
        {
            priorityTasks.push_back(task);
        }
        else
        {
            normalTasks.push_back(task);
        }
    }

    // Display all tasks
    void displayTasks() const
    {
        std::cout << "\n=== Priority Tasks ===\n";
        displayTaskList(priorityTasks);

        std::cout << "\n=== Normal Tasks ===\n";
        displayTaskList(normalTasks);
    }

    // Helper function to display tasks
    template <typename Container>
    void displayTaskList(const Container &tasks) const
    {
        int index = 1;
        for (const auto &task : tasks)
        {
            std::cout << "\n"
                      << index++ << ". Title: " << task.getTitle()
                      << "\n   Description: " << task.getDescription()
                      << "\n   Priority: " << task.getPriority()
                      << "\n   Due Date: " << task.getDueDate()
                      << "\n   Status: " << (task.isCompleted() ? "Completed" : "Pending")
                      << "\n";
        }
    }

    // Mark task as completed
    bool markTaskCompleted(const std::string &title)
    {
        auto markComplete = [&title](std::vector<Task> &tasks) -> bool
        {
            for (auto &task : tasks)
            {
                if (task.getTitle() == title)
                {
                    task.setCompleted(true);
                    return true;
                }
            }
            return false;
        };

        if (markComplete(normalTasks))
            return true;

        for (auto &task : priorityTasks)
        {
            if (task.getTitle() == title)
            {
                task.setCompleted(true);
                return true;
            }
        }
        return false;
    }

    // Remove completed tasks
    void removeCompletedTasks()
    {
        auto removeCompleted = [](auto &tasks)
        {
            tasks.erase(
                std::remove_if(tasks.begin(), tasks.end(),
                               [](const Task &task)
                               { return task.isCompleted(); }),
                tasks.end());
        };

        removeCompleted(normalTasks);
        removeCompleted(priorityTasks);
    }
};

int main()
{
    TaskManager manager;
    int choice;

    do
    {
        std::cout << "\n=== Task Manager Menu ===\n"
                  << "1. Add New Task\n"
                  << "2. Display All Tasks\n"
                  << "3. Mark Task as Completed\n"
                  << "4. Remove Completed Tasks\n"
                  << "5. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
        {
            std::string title, description, priority, dueDate;

            std::cout << "Enter task title: ";
            std::getline(std::cin, title);

            std::cout << "Enter task description: ";
            std::getline(std::cin, description);

            std::cout << "Enter priority (High/Medium/Low): ";
            std::getline(std::cin, priority);

            std::cout << "Enter due date (DD/MM/YYYY): ";
            std::getline(std::cin, dueDate);

            Task newTask(title, description, priority, dueDate);
            manager.addTask(newTask);
            std::cout << "Task added successfully!\n";
            break;
        }
        case 2:
        {
            manager.displayTasks();
            break;
        }
        case 3:
        {
            std::string title;
            std::cout << "Enter task title to mark as completed: ";
            std::getline(std::cin, title);

            if (manager.markTaskCompleted(title))
            {
                std::cout << "Task marked as completed!\n";
            }
            else
            {
                std::cout << "Task not found!\n";
            }
            break;
        }
        case 4:
        {
            manager.removeCompletedTasks();
            std::cout << "Completed tasks removed!\n";
            break;
        }
        case 5:
        {
            std::cout << "Thank you for using Task Manager!\n";
            break;
        }
        default:
        {
            std::cout << "Invalid choice! Please try again.\n";
        }
        }
    } while (choice != 5);

    return 0;
=======
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>

class Task
{
private:
    std::string title;
    std::string description;
    std::string priority;
    std::string dueDate;
    bool completed;

public:
    Task(const std::string &t, const std::string &d, const std::string &p, const std::string &dd)
        : title(t), description(d), priority(p), dueDate(dd), completed(false) {}

    // Getters
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::string getPriority() const { return priority; }
    std::string getDueDate() const { return dueDate; }
    bool isCompleted() const { return completed; }

    // Setters
    void setCompleted(bool status) { completed = status; }
    void setTitle(const std::string &t) { title = t; }
    void setDescription(const std::string &d) { description = d; }
    void setPriority(const std::string &p) { priority = p; }
    void setDueDate(const std::string &dd) { dueDate = dd; }
};

class TaskManager
{
private:
    std::vector<Task> normalTasks;  // For regular tasks
    std::deque<Task> priorityTasks; // For priority tasks that need quick front/back access

public:
    // Add a new task
    void addTask(const Task &task)
    {
        if (task.getPriority() == "High")
        {
            priorityTasks.push_front(task);
        }
        else if (task.getPriority() == "Low")
        {
            priorityTasks.push_back(task);
        }
        else
        {
            normalTasks.push_back(task);
        }
    }

    // Display all tasks
    void displayTasks() const
    {
        std::cout << "\n=== Priority Tasks ===\n";
        displayTaskList(priorityTasks);

        std::cout << "\n=== Normal Tasks ===\n";
        displayTaskList(normalTasks);
    }

    // Helper function to display tasks
    template <typename Container>
    void displayTaskList(const Container &tasks) const
    {
        int index = 1;
        for (const auto &task : tasks)
        {
            std::cout << "\n"
                      << index++ << ". Title: " << task.getTitle()
                      << "\n   Description: " << task.getDescription()
                      << "\n   Priority: " << task.getPriority()
                      << "\n   Due Date: " << task.getDueDate()
                      << "\n   Status: " << (task.isCompleted() ? "Completed" : "Pending")
                      << "\n";
        }
    }

    // Mark task as completed
    bool markTaskCompleted(const std::string &title)
    {
        auto markComplete = [&title](std::vector<Task> &tasks) -> bool
        {
            for (auto &task : tasks)
            {
                if (task.getTitle() == title)
                {
                    task.setCompleted(true);
                    return true;
                }
            }
            return false;
        };

        if (markComplete(normalTasks))
            return true;

        for (auto &task : priorityTasks)
        {
            if (task.getTitle() == title)
            {
                task.setCompleted(true);
                return true;
            }
        }
        return false;
    }

    // Remove completed tasks
    void removeCompletedTasks()
    {
        auto removeCompleted = [](auto &tasks)
        {
            tasks.erase(
                std::remove_if(tasks.begin(), tasks.end(),
                               [](const Task &task)
                               { return task.isCompleted(); }),
                tasks.end());
        };

        removeCompleted(normalTasks);
        removeCompleted(priorityTasks);
    }
};

int main()
{
    TaskManager manager;
    int choice;

    do
    {
        std::cout << "\n=== Task Manager Menu ===\n"
                  << "1. Add New Task\n"
                  << "2. Display All Tasks\n"
                  << "3. Mark Task as Completed\n"
                  << "4. Remove Completed Tasks\n"
                  << "5. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
        {
            std::string title, description, priority, dueDate;

            std::cout << "Enter task title: ";
            std::getline(std::cin, title);

            std::cout << "Enter task description: ";
            std::getline(std::cin, description);

            std::cout << "Enter priority (High/Medium/Low): ";
            std::getline(std::cin, priority);

            std::cout << "Enter due date (DD/MM/YYYY): ";
            std::getline(std::cin, dueDate);

            Task newTask(title, description, priority, dueDate);
            manager.addTask(newTask);
            std::cout << "Task added successfully!\n";
            break;
        }
        case 2:
        {
            manager.displayTasks();
            break;
        }
        case 3:
        {
            std::string title;
            std::cout << "Enter task title to mark as completed: ";
            std::getline(std::cin, title);

            if (manager.markTaskCompleted(title))
            {
                std::cout << "Task marked as completed!\n";
            }
            else
            {
                std::cout << "Task not found!\n";
            }
            break;
        }
        case 4:
        {
            manager.removeCompletedTasks();
            std::cout << "Completed tasks removed!\n";
            break;
        }
        case 5:
        {
            std::cout << "Thank you for using Task Manager!\n";
            break;
        }
        default:
        {
            std::cout << "Invalid choice! Please try again.\n";
        }
        }
    } while (choice != 5);

    return 0;
>>>>>>> 31e08e23f34c54259e619d4f92b64e13a13828aa
}