#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <deque>
#include <algorithm>
#include <sstream>
#include <chrono>
#include "Task.h"

class TaskManager
{
private:
    std::vector<Task> tasks;      // Store regular tasks
    std::deque<Task> urgentTasks; // Store urgent tasks that need quick access from both ends

public:
    // Add a regular task to the vector
    void addTask(const Task &task)
    {
        tasks.push_back(task);
    }

    // Add an urgent task (can be added to front or back of deque)
    void addUrgentTask(const Task &task, bool addToFront = true)
    {
        if (addToFront)
        {
            urgentTasks.push_front(task);
        }
        else
        {
            urgentTasks.push_back(task);
        }
    }

    // Create task from parameters and add to regular tasks
    void addTask(const std::string &title, const std::string &description,
                 const std::string &dueDate, int priority, bool isUrgent = false)
    {
        // Parse date string (YYYY-MM-DD format)
        std::tm tm = {};
        std::istringstream ss(dueDate);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        Task newTask(title, description, timePoint, priority);

        if (isUrgent)
        {
            addUrgentTask(newTask, true);
        }
        else
        {
            addTask(newTask);
        }
    }

    // Remove a task by title from regular tasks
    bool removeTask(const std::string &title)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [&title](const Task &task)
                               { return task.getTitle() == title; });

        if (it != tasks.end())
        {
            tasks.erase(it);
            return true;
        }
        return false;
    }

    // Remove a task by ID from regular tasks
    bool removeTaskById(int id)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [id](const Task &task)
                               { return task.getId() == id; });

        if (it != tasks.end())
        {
            tasks.erase(it);
            return true;
        }
        return false;
    }

    // Remove an urgent task by title
    bool removeUrgentTask(const std::string &title)
    {
        auto it = std::find_if(urgentTasks.begin(), urgentTasks.end(),
                               [&title](const Task &task)
                               { return task.getTitle() == title; });

        if (it != urgentTasks.end())
        {
            urgentTasks.erase(it);
            return true;
        }
        return false;
    }

    // Remove an urgent task by ID
    bool removeUrgentTaskById(int id)
    {
        auto it = std::find_if(urgentTasks.begin(), urgentTasks.end(),
                               [id](const Task &task)
                               { return task.getId() == id; });

        if (it != urgentTasks.end())
        {
            urgentTasks.erase(it);
            return true;
        }
        return false;
    }

    // Mark a task as completed by ID
    bool markTaskCompleted(int id)
    {
        // Check regular tasks
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [id](Task &task)
                               { return task.getId() == id; });

        if (it != tasks.end())
        {
            it->setCompleted(true);
            return true;
        }

        // Check urgent tasks
        auto urgentIt = std::find_if(urgentTasks.begin(), urgentTasks.end(),
                                     [id](Task &task)
                                     { return task.getId() == id; });

        if (urgentIt != urgentTasks.end())
        {
            urgentIt->setCompleted(true);
            return true;
        }

        return false;
    }

    // Mark a task as completed
    bool markTaskCompleted(const std::string &title)
    {
        // Check regular tasks
        auto it = std::find_if(tasks.begin(), tasks.end(),
                               [&title](Task &task)
                               { return task.getTitle() == title; });

        if (it != tasks.end())
        {
            it->setCompleted(true);
            return true;
        }

        // Check urgent tasks
        auto urgentIt = std::find_if(urgentTasks.begin(), urgentTasks.end(),
                                     [&title](Task &task)
                                     { return task.getTitle() == title; });

        if (urgentIt != urgentTasks.end())
        {
            urgentIt->setCompleted(true);
            return true;
        }

        return false;
    }

    // Display all tasks
    void displayAllTasks() const
    {
        std::cout << "\n=== Regular Tasks ===\n";
        for (const auto &task : tasks)
        {
            task.display();
        }

        std::cout << "\n=== Urgent Tasks ===\n";
        for (const auto &task : urgentTasks)
        {
            task.display();
        }
    }

    // Sort regular tasks by priority (high to low)
    void sortTasksByPriority()
    {
        std::sort(tasks.begin(), tasks.end(),
                  [](const Task &a, const Task &b)
                  { return a.getPriority() > b.getPriority(); });
    }

    // Sort tasks by due date (earliest first)
    void sortTasksByDueDate()
    {
        std::sort(tasks.begin(), tasks.end(),
                  [](const Task &a, const Task &b)
                  { return a.getDueDate() < b.getDueDate(); });
    }

    // Sort tasks by title (alphabetical)
    void sortTasksByTitle()
    {
        std::sort(tasks.begin(), tasks.end(),
                  [](const Task &a, const Task &b)
                  { return a.getTitle() < b.getTitle(); });
    }

    // Get all tasks as JSON
    std::string getAllTasksJson() const
    {
        std::ostringstream json;
        json << "{\"regularTasks\":[";

        for (size_t i = 0; i < tasks.size(); ++i)
        {
            if (i > 0)
                json << ",";
            json << tasks[i].toJson();
        }

        json << "],\"urgentTasks\":[";

        for (size_t i = 0; i < urgentTasks.size(); ++i)
        {
            if (i > 0)
                json << ",";
            json << urgentTasks[i].toJson();
        }

        json << "]}";
        return json.str();
    }

    // Get number of tasks
    size_t getRegularTaskCount() const { return tasks.size(); }
    size_t getUrgentTaskCount() const { return urgentTasks.size(); }

    // Get all tasks (both regular and urgent) as a single vector for sorting
    std::vector<Task> getAllTasks() const
    {
        std::vector<Task> allTasks;
        allTasks.insert(allTasks.end(), tasks.begin(), tasks.end());
        allTasks.insert(allTasks.end(), urgentTasks.begin(), urgentTasks.end());
        return allTasks;
    }
};

#endif // TASKMANAGER_H