#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <map>
#include <regex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include "TaskManager.h"

class SimpleHttpServer
{
private:
    int port;
    TaskManager taskManager;

#ifdef _WIN32
    SOCKET server_socket;
#else
    int server_socket;
#endif

    std::string urlDecode(const std::string &str)
    {
        std::string result;
        std::istringstream iss(str);
        char c;
        while (iss >> c)
        {
            if (c == '%')
            {
                int hex;
                iss >> std::hex >> hex;
                result += static_cast<char>(hex);
            }
            else if (c == '+')
            {
                result += ' ';
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    std::map<std::string, std::string> parseFormData(const std::string &data)
    {
        std::map<std::string, std::string> params;
        std::istringstream iss(data);
        std::string pair;

        while (std::getline(iss, pair, '&'))
        {
            size_t pos = pair.find('=');
            if (pos != std::string::npos)
            {
                std::string key = urlDecode(pair.substr(0, pos));
                std::string value = urlDecode(pair.substr(pos + 1));
                params[key] = value;
            }
        }
        return params;
    }

    std::string generateTaskHtml(const std::vector<Task> &regularTasks, const std::deque<Task> &urgentTasks)
    {
        std::ostringstream html;

        html << "<div id=\"task-list\">";

        // Urgent tasks
        if (!urgentTasks.empty())
        {
            html << "<h3 class=\"urgent-header\">🚨 Urgent Tasks (using deque)</h3>";
            for (const auto &task : urgentTasks)
            {
                html << "<div class=\"task-item urgent-task\">";
                html << "<div class=\"task-header\">";
                html << "<h4>" << task.getTitle() << "</h4>";
                html << "<span class=\"priority\">Priority: " << task.getPriority() << "</span>";
                html << "</div>";
                html << "<p>" << task.getDescription() << "</p>";
                html << "<div class=\"task-meta\">";
                html << "<span>Due: " << task.getDueDateString() << "</span>";
                html << "<span class=\"status " << (task.isCompleted() ? "completed" : "pending") << "\">";
                html << (task.isCompleted() ? "✓ Completed" : "⏳ Pending") << "</span>";
                html << "</div>";
                html << "<div class=\"task-actions\">";
                if (!task.isCompleted())
                {
                    html << "<button hx-post=\"/complete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"complete-btn\">Complete</button>";
                }
                html << "<button hx-post=\"/delete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"delete-btn\">Delete</button>";
                html << "</div></div>";
            }
        }

        // Regular tasks
        if (!regularTasks.empty())
        {
            html << "<h3 class=\"regular-header\">📝 Regular Tasks (using vector)</h3>";
            for (const auto &task : regularTasks)
            {
                html << "<div class=\"task-item regular-task\">";
                html << "<div class=\"task-header\">";
                html << "<h4>" << task.getTitle() << "</h4>";
                html << "<span class=\"priority\">Priority: " << task.getPriority() << "</span>";
                html << "</div>";
                html << "<p>" << task.getDescription() << "</p>";
                html << "<div class=\"task-meta\">";
                html << "<span>Due: " << task.getDueDateString() << "</span>";
                html << "<span class=\"status " << (task.isCompleted() ? "completed" : "pending") << "\">";
                html << (task.isCompleted() ? "✓ Completed" : "⏳ Pending") << "</span>";
                html << "</div>";
                html << "<div class=\"task-actions\">";
                if (!task.isCompleted())
                {
                    html << "<button hx-post=\"/complete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"complete-btn\">Complete</button>";
                }
                html << "<button hx-post=\"/delete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"delete-btn\">Delete</button>";
                html << "</div></div>";
            }
        }

        if (regularTasks.empty() && urgentTasks.empty())
        {
            html << "<div class=\"no-tasks\">No tasks yet. Add your first task above!</div>";
        }

        html << "</div>";
        return html.str();
    }

public:
    SimpleHttpServer(int p) : port(p)
    {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        // Add some sample tasks
        taskManager.addTask("Sample Task", "This is a sample regular task", "2025-01-15", 2, false);
        taskManager.addTask("Urgent Bug Fix", "Critical production issue", "2025-01-10", 5, true);
    }

    ~SimpleHttpServer()
    {
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
    }

    void start()
    {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        bind(server_socket, (struct sockaddr *)&address, sizeof(address));
        listen(server_socket, 3);

        std::cout << "Task Manager Server running on http://localhost:" << port << std::endl;
        std::cout << "Visit the URL in your browser to access the task manager!" << std::endl;

        while (true)
        {
#ifdef _WIN32
            SOCKET client_socket = accept(server_socket, nullptr, nullptr);
#else
            int client_socket = accept(server_socket, nullptr, nullptr);
#endif

            std::thread clientThread(&SimpleHttpServer::handleClient, this, client_socket);
            clientThread.detach();
        }
    }

private:
#ifdef _WIN32
    void handleClient(SOCKET client_socket){
#else
    void handleClient(int client_socket)
    {
#endif
        char buffer[4096] = { 0 };
    recv(client_socket, buffer, 4096, 0);

    std::string request(buffer);
    std::string response = processRequest(request);

    send(client_socket, response.c_str(), response.length(), 0);

#ifdef _WIN32
    closesocket(client_socket);
#else
        close(client_socket);
#endif
}

std::string
processRequest(const std::string &request)
{
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;

    if (method == "GET" && path == "/")
    {
        return getIndexPage();
    }
    else if (method == "GET" && path == "/tasks")
    {
        return getTasksHtml();
    }
    else if (method == "POST" && path == "/add-task")
    {
        return handleAddTask(request);
    }
    else if (method == "POST" && path == "/complete-task")
    {
        return handleCompleteTask(request);
    }
    else if (method == "POST" && path == "/delete-task")
    {
        return handleDeleteTask(request);
    }
    else if (method == "POST" && path == "/sort-tasks")
    {
        return handleSortTasks(request);
    }

    return get404Page();
}

std::string getIndexPage()
{
    std::string html = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>C++ Task Manager - STL Sequential Containers Demo</title>
    <script src="https://unpkg.com/htmx.org@1.9.10"></script>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container { 
            max-width: 1200px; 
            margin: 0 auto; 
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #2c3e50 0%, #34495e 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        .header h1 { 
            font-size: 2.5em; 
            margin-bottom: 10px;
            font-weight: 300;
        }
        .stl-info {
            background: #f8f9fa;
            padding: 20px;
            border-left: 4px solid #3498db;
            margin: 20px;
            border-radius: 8px;
        }
        .stl-info h3 { color: #2c3e50; margin-bottom: 10px; }
        .stl-info p { color: #7f8c8d; line-height: 1.6; }
        .form-section {
            padding: 30px;
            background: #ffffff;
        }
        .form-row {
            display: flex;
            gap: 15px;
            margin-bottom: 15px;
            flex-wrap: wrap;
        }
        .form-group {
            flex: 1;
            min-width: 200px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: 600;
            color: #2c3e50;
        }
        input, textarea, select {
            width: 100%;
            padding: 12px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s ease;
        }
        input:focus, textarea:focus, select:focus {
            outline: none;
            border-color: #3498db;
            box-shadow: 0 0 0 3px rgba(52, 152, 219, 0.1);
        }
        .btn {
            background: linear-gradient(135deg, #3498db 0%, #2980b9 100%);
            color: white;
            padding: 12px 25px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 16px;
            font-weight: 600;
            transition: all 0.3s ease;
            margin-right: 10px;
            margin-bottom: 10px;
        }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 20px rgba(52, 152, 219, 0.3);
        }
        .sort-controls {
            background: #f8f9fa;
            padding: 20px;
            border-bottom: 1px solid #e0e0e0;
            text-align: center;
        }
        .sort-controls h3 {
            margin-bottom: 15px;
            color: #2c3e50;
        }
        .task-item {
            background: white;
            margin: 15px 20px;
            padding: 20px;
            border-radius: 12px;
            border-left: 4px solid #3498db;
            box-shadow: 0 4px 15px rgba(0,0,0,0.08);
            transition: all 0.3s ease;
        }
        .task-item:hover {
            transform: translateY(-3px);
            box-shadow: 0 8px 25px rgba(0,0,0,0.15);
        }
        .urgent-task { border-left-color: #e74c3c; }
        .regular-task { border-left-color: #3498db; }
        .task-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }
        .task-header h4 {
            color: #2c3e50;
            font-size: 1.3em;
        }
        .priority {
            background: #ecf0f1;
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 0.9em;
            font-weight: 600;
            color: #2c3e50;
        }
        .task-meta {
            display: flex;
            justify-content: space-between;
            margin: 15px 0;
            font-size: 0.9em;
            color: #7f8c8d;
        }
        .status.completed { color: #27ae60; font-weight: 600; }
        .status.pending { color: #f39c12; font-weight: 600; }
        .task-actions {
            display: flex;
            gap: 10px;
            margin-top: 15px;
        }
        .complete-btn {
            background: linear-gradient(135deg, #27ae60 0%, #2ecc71 100%);
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 6px;
            cursor: pointer;
            font-size: 14px;
        }
        .delete-btn {
            background: linear-gradient(135deg, #e74c3c 0%, #c0392b 100%);
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 6px;
            cursor: pointer;
            font-size: 14px;
        }
        .urgent-header, .regular-header {
            color: #2c3e50;
            margin: 25px 20px 15px 20px;
            font-size: 1.4em;
            border-bottom: 2px solid #ecf0f1;
            padding-bottom: 10px;
        }
        .no-tasks {
            text-align: center;
            color: #7f8c8d;
            font-style: italic;
            padding: 40px;
            font-size: 1.1em;
        }
        .container-info {
            background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%);
            color: white;
            padding: 15px 20px;
            margin: 0 20px 20px 20px;
            border-radius: 8px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 C++ Task Manager</h1>
            <p>Demonstrating STL Sequential Containers: std::vector & std::deque</p>
        </div>

        <div class="stl-info">
            <h3>📚 STL Sequential Containers in Action</h3>
            <p><strong>std::vector&lt;Task&gt;</strong> - Used for regular tasks: Dynamic array with efficient random access and fast insertion at the end.</p>
            <p><strong>std::deque&lt;Task&gt;</strong> - Used for urgent tasks: Double-ended queue allowing efficient insertion/removal at both front and back.</p>
        </div>

        <div class="form-section">
            <h2>➕ Add New Task</h2>
            <form hx-post="/add-task" hx-target="#task-list" hx-trigger="submit">
                <div class="form-row">
                    <div class="form-group">
                        <label for="title">Task Title</label>
                        <input type="text" id="title" name="title" required>
                    </div>
                    <div class="form-group">
                        <label for="priority">Priority (1-5)</label>
                        <select id="priority" name="priority" required>
                            <option value="1">1 - Low</option>
                            <option value="2">2 - Normal</option>
                            <option value="3" selected>3 - Medium</option>
                            <option value="4">4 - High</option>
                            <option value="5">5 - Critical</option>
                        </select>
                    </div>
                </div>
                <div class="form-row">
                    <div class="form-group">
                        <label for="description">Description</label>
                        <textarea id="description" name="description" rows="3" required></textarea>
                    </div>
                    <div class="form-group">
                        <label for="dueDate">Due Date</label>
                        <input type="date" id="dueDate" name="dueDate" required>
                        <br><br>
                        <label>
                            <input type="checkbox" name="isUrgent" value="true"> 
                            🚨 Mark as Urgent (use deque)
                        </label>
                    </div>
                </div>
                <button type="submit" class="btn">Add Task</button>
            </form>
        </div>

        <div class="sort-controls">
            <h3>🔧 Sort Tasks</h3>
            <button hx-post="/sort-tasks" hx-vals='{"sortBy":"priority"}' hx-target="#task-list" class="btn">Sort by Priority</button>
            <button hx-post="/sort-tasks" hx-vals='{"sortBy":"dueDate"}' hx-target="#task-list" class="btn">Sort by Due Date</button>
            <button hx-post="/sort-tasks" hx-vals='{"sortBy":"title"}' hx-target="#task-list" class="btn">Sort by Title</button>
        </div>

        <div class="container-info">
            <strong>💡 Container Usage:</strong> Regular tasks are stored in a std::vector for efficient sequential access and fast end insertions. Urgent tasks use std::deque to allow fast insertion at both front and back.
        </div>

        <div hx-get="/tasks" hx-trigger="load" hx-target="this">
            Loading tasks...
        </div>
    </div>
</body>
</html>)";

    return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " +
           std::to_string(html.length()) + "\r\n\r\n" + html;
}

std::string getTasksHtml()
{
    std::vector<Task> regularTasks;
    std::deque<Task> urgentTasks;

    // Get tasks from manager (we need to access private members)
    // For now, we'll use the getAllTasksJson and parse or create a simplified version
    auto allTasks = taskManager.getAllTasks();

    // Separate into regular and urgent based on priority
    for (const auto &task : allTasks)
    {
        if (task.getPriority() >= 4)
        {
            urgentTasks.push_back(task);
        }
        else
        {
            regularTasks.push_back(task);
        }
    }

    std::string html = generateTaskHtml(regularTasks, urgentTasks);

    return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " +
           std::to_string(html.length()) + "\r\n\r\n" + html;
}

std::string handleAddTask(const std::string &request)
{
    // Extract form data from POST body
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        return get404Page();

    std::string body = request.substr(bodyStart + 4);
    auto params = parseFormData(body);

    std::string title = params["title"];
    std::string description = params["description"];
    std::string dueDate = params["dueDate"];
    int priority = std::stoi(params["priority"]);
    bool isUrgent = params.find("isUrgent") != params.end();

    taskManager.addTask(title, description, dueDate, priority, isUrgent);

    return getTasksHtml();
}

std::string handleCompleteTask(const std::string &request)
{
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        return get404Page();

    std::string body = request.substr(bodyStart + 4);
    auto params = parseFormData(body);

    int id = std::stoi(params["id"]);
    taskManager.markTaskCompleted(id);

    return getTasksHtml();
}

std::string handleDeleteTask(const std::string &request)
{
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        return get404Page();

    std::string body = request.substr(bodyStart + 4);
    auto params = parseFormData(body);

    int id = std::stoi(params["id"]);
    taskManager.removeTaskById(id);
    taskManager.removeUrgentTaskById(id);

    return getTasksHtml();
}

std::string handleSortTasks(const std::string &request)
{
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart == std::string::npos)
        return get404Page();

    std::string body = request.substr(bodyStart + 4);
    auto params = parseFormData(body);

    std::string sortBy = params["sortBy"];

    if (sortBy == "priority")
    {
        taskManager.sortTasksByPriority();
    }
    else if (sortBy == "dueDate")
    {
        taskManager.sortTasksByDueDate();
    }
    else if (sortBy == "title")
    {
        taskManager.sortTasksByTitle();
    }

    return getTasksHtml();
}

std::string get404Page()
{
    std::string html = "<h1>404 Not Found</h1>";
    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " +
           std::to_string(html.length()) + "\r\n\r\n" + html;
}
}
;

int main()
{
    SimpleHttpServer server(8080);
    server.start();
    return 0;
}