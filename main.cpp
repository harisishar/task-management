#include <iostream>
#include <chrono>
#include "TaskManager.h"

using namespace std;

// Helper function to create a future date
chrono::system_clock::time_point createDueDate(int daysFromNow)
{
    return chrono::system_clock::now() + chrono::hours(24 * daysFromNow);
}

int main()
{
    TaskManager manager;

    // Adding regular tasks
    Task task1("Complete Report", "Write quarterly report", createDueDate(5), 2);
    Task task2("Review Code", "Review team's pull requests", createDueDate(2), 3);
    Task task3("Update Documentation", "Update API documentation", createDueDate(7), 1);

    manager.addTask(task1);
    manager.addTask(task2);
    manager.addTask(task3);

    // Adding urgent tasks
    Task urgentTask1("Fix Production Bug", "Critical bug in payment system", createDueDate(1), 5);
    Task urgentTask2("Client Meeting Prep", "Prepare presentation for client", createDueDate(1), 4);

    manager.addUrgentTask(urgentTask1, true);  // Add to front
    manager.addUrgentTask(urgentTask2, false); // Add to back

    cout << "Initial Task List:\n";
    manager.displayAllTasks();

    // Demonstrate task management operations
    cout << "\nMarking 'Review Code' as completed...\n";
    manager.markTaskCompleted("Review Code");

    cout << "\nSorting regular tasks by priority...\n";
    manager.sortTasksByPriority();

    cout << "\nUpdated Task List:\n";
    manager.displayAllTasks();

    cout << "\nRemoving 'Fix Production Bug' from urgent tasks...\n";
    manager.removeUrgentTask("Fix Production Bug");

    cout << "\nFinal Task List:\n";
    manager.displayAllTasks();

    cout << "\nTask Statistics:\n";
    cout << "Regular Tasks: " << manager.getRegularTaskCount() << endl;
    cout << "Urgent Tasks: " << manager.getUrgentTaskCount() << endl;

    return 0;
}