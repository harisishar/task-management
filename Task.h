#ifndef TASK_H
#define TASK_H

#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>

class Task
{
private:
    static int nextId;
    int id;
    std::string title;
    std::string description;
    bool completed;
    std::chrono::system_clock::time_point dueDate;
    int priority;

public:
    Task(const std::string &t, const std::string &desc, const std::chrono::system_clock::time_point &due, int prio)
        : id(nextId++), title(t), description(desc), completed(false), dueDate(due), priority(prio) {}

    // Getters
    int getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    bool isCompleted() const { return completed; }
    std::chrono::system_clock::time_point getDueDate() const { return dueDate; }
    int getPriority() const { return priority; }

    // Setters
    void setTitle(const std::string &t) { title = t; }
    void setDescription(const std::string &desc) { description = desc; }
    void setCompleted(bool status) { completed = status; }
    void setDueDate(const std::chrono::system_clock::time_point &due) { dueDate = due; }
    void setPriority(int prio) { priority = prio; }

    // Helper method to format date as string
    std::string getDueDateString() const
    {
        std::time_t time = std::chrono::system_clock::to_time_t(dueDate);
        std::tm *tm = std::localtime(&time);
        std::ostringstream oss;
        oss << std::put_time(tm, "%Y-%m-%d");
        return oss.str();
    }

    // Convert to JSON string for API
    std::string toJson() const
    {
        std::ostringstream json;
        json << "{"
             << "\"id\":" << id << ","
             << "\"title\":\"" << title << "\","
             << "\"description\":\"" << description << "\","
             << "\"completed\":" << (completed ? "true" : "false") << ","
             << "\"dueDate\":\"" << getDueDateString() << "\","
             << "\"priority\":" << priority
             << "}";
        return json.str();
    }

    // Display task information
    void display() const
    {
        std::time_t time = std::chrono::system_clock::to_time_t(dueDate);
        std::cout << "ID: " << id << "\n"
                  << "Title: " << title << "\n"
                  << "Description: " << description << "\n"
                  << "Status: " << (completed ? "Completed" : "Pending") << "\n"
                  << "Due Date: " << std::ctime(&time)
                  << "Priority: " << priority << "\n"
                  << "------------------------\n";
    }
};

#endif // TASK_H