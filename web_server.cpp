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

        html << "<div id=\"task-list\" class=\"kanban-board\">";

        // Kanban columns container
        html << "<div class=\"kanban-columns\">";

        // Urgent tasks column
        html << "<div class=\"kanban-column urgent-column\">";
        html << "<div class=\"column-header urgent-header\">";
        html << "<h3>🚨 Urgent Tasks</h3>";
        html << "<span class=\"container-type\">(using std::deque)</span>";
        html << "<span class=\"task-count\">" << urgentTasks.size() << " tasks</span>";
        html << "</div>";
        html << "<div class=\"column-content\">";

        if (!urgentTasks.empty())
        {
            for (const auto &task : urgentTasks)
            {
                html << "<div class=\"task-card urgent-task\">";
                html << "<div class=\"task-header\">";
                html << "<h4>" << task.getTitle() << "</h4>";
                html << "<span class=\"priority priority-" << task.getPriority() << "\">P" << task.getPriority() << "</span>";
                html << "</div>";
                html << "<p class=\"task-description\">" << task.getDescription() << "</p>";
                html << "<div class=\"task-meta\">";
                html << "<span class=\"due-date\">📅 " << task.getDueDateString() << "</span>";
                html << "<span class=\"status " << (task.isCompleted() ? "completed" : "pending") << "\">";
                html << (task.isCompleted() ? "✅ Done" : "⏳ Active") << "</span>";
                html << "</div>";
                html << "<div class=\"task-actions\">";
                if (!task.isCompleted())
                {
                    html << "<button hx-post=\"/complete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"action-btn complete-btn\">✓ Complete</button>";
                }
                html << "<button hx-post=\"/delete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"action-btn delete-btn\">🗑️ Delete</button>";
                html << "</div>";
                html << "</div>";
            }
        }
        else
        {
            html << "<div class=\"empty-column\">No urgent tasks</div>";
        }
        html << "</div></div>";

        // Regular tasks column
        html << "<div class=\"kanban-column regular-column\">";
        html << "<div class=\"column-header regular-header\">";
        html << "<h3>📝 Regular Tasks</h3>";
        html << "<span class=\"container-type\">(using std::vector)</span>";
        html << "<span class=\"task-count\">" << regularTasks.size() << " tasks</span>";
        html << "</div>";
        html << "<div class=\"column-content\">";

        if (!regularTasks.empty())
        {
            for (const auto &task : regularTasks)
            {
                html << "<div class=\"task-card regular-task\">";
                html << "<div class=\"task-header\">";
                html << "<h4>" << task.getTitle() << "</h4>";
                html << "<span class=\"priority priority-" << task.getPriority() << "\">P" << task.getPriority() << "</span>";
                html << "</div>";
                html << "<p class=\"task-description\">" << task.getDescription() << "</p>";
                html << "<div class=\"task-meta\">";
                html << "<span class=\"due-date\">📅 " << task.getDueDateString() << "</span>";
                html << "<span class=\"status " << (task.isCompleted() ? "completed" : "pending") << "\">";
                html << (task.isCompleted() ? "✅ Done" : "⏳ Active") << "</span>";
                html << "</div>";
                html << "<div class=\"task-actions\">";
                if (!task.isCompleted())
                {
                    html << "<button hx-post=\"/complete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"action-btn complete-btn\">✓ Complete</button>";
                }
                html << "<button hx-post=\"/delete-task\" hx-vals='{\"id\":" << task.getId() << "}' hx-target=\"#task-list\" class=\"action-btn delete-btn\">🗑️ Delete</button>";
                html << "</div>";
                html << "</div>";
            }
        }
        else
        {
            html << "<div class=\"empty-column\">No regular tasks</div>";
        }
        html << "</div></div>";

        html << "</div>"; // Close kanban-columns

        if (regularTasks.empty() && urgentTasks.empty())
        {
            html << "<div class=\"no-tasks\">🎯 Your Kanban board is empty! Add your first task above to get started.</div>";
        }

        html << "</div>"; // Close kanban-board
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
        
        /* Kanban Board Styles */
        .kanban-board {
            margin: 20px;
        }
        .kanban-columns {
            display: flex;
            gap: 20px;
            min-height: 500px;
            align-items: flex-start;
        }
        .kanban-column {
            flex: 1;
            background: #f8f9fa;
            border-radius: 12px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
            min-height: 400px;
            display: flex;
            flex-direction: column;
        }
        .urgent-column {
            border-top: 4px solid #e74c3c;
        }
        .regular-column {
            border-top: 4px solid #3498db;
        }
        .column-header {
            padding: 20px;
            background: white;
            border-radius: 12px 12px 0 0;
            border-bottom: 2px solid #ecf0f1;
            text-align: center;
        }
        .column-header h3 {
            margin: 0 0 8px 0;
            font-size: 1.3em;
            color: #2c3e50;
        }
        .container-type {
            display: block;
            font-size: 0.85em;
            color: #7f8c8d;
            font-style: italic;
            margin-bottom: 8px;
        }
        .task-count {
            display: inline-block;
            background: #ecf0f1;
            padding: 4px 12px;
            border-radius: 15px;
            font-size: 0.8em;
            font-weight: 600;
            color: #2c3e50;
        }
        .column-content {
            flex: 1;
            padding: 15px;
            overflow-y: auto;
            max-height: 600px;
        }
        .task-card {
            background: white;
            margin-bottom: 15px;
            padding: 18px;
            border-radius: 10px;
            box-shadow: 0 3px 12px rgba(0,0,0,0.08);
            transition: all 0.3s ease;
            border-left: 4px solid transparent;
        }
        .task-card:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(0,0,0,0.15);
        }
        .urgent-task { 
            border-left-color: #e74c3c;
            background: linear-gradient(135deg, #fff 0%, #fdf2f2 100%);
        }
        .regular-task { 
            border-left-color: #3498db;
            background: linear-gradient(135deg, #fff 0%, #f2f8fd 100%);
        }
        .task-header {
            display: flex;
            justify-content: space-between;
            align-items: flex-start;
            margin-bottom: 12px;
        }
        .task-header h4 {
            color: #2c3e50;
            font-size: 1.1em;
            margin: 0;
            flex: 1;
            margin-right: 10px;
            line-height: 1.3;
        }
        .priority {
            padding: 4px 8px;
            border-radius: 12px;
            font-size: 0.75em;
            font-weight: 700;
            color: white;
            min-width: 28px;
            text-align: center;
        }
        .priority-1 { background: #95a5a6; }
        .priority-2 { background: #3498db; }
        .priority-3 { background: #f39c12; }
        .priority-4 { background: #e67e22; }
        .priority-5 { background: #e74c3c; }
        .task-description {
            color: #5a6c7d;
            font-size: 0.9em;
            line-height: 1.4;
            margin: 0 0 12px 0;
        }
        .task-meta {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 12px 0;
            font-size: 0.85em;
            flex-wrap: wrap;
            gap: 8px;
        }
        .due-date {
            color: #7f8c8d;
            font-weight: 500;
        }
        .status.completed { 
            color: #27ae60; 
            font-weight: 600;
            background: #d5f4e6;
            padding: 2px 8px;
            border-radius: 10px;
            font-size: 0.8em;
        }
        .status.pending { 
            color: #f39c12; 
            font-weight: 600;
            background: #fef9e7;
            padding: 2px 8px;
            border-radius: 10px;
            font-size: 0.8em;
        }
        .task-actions {
            display: flex;
            gap: 8px;
            margin-top: 15px;
        }
        .action-btn {
            flex: 1;
            padding: 8px 12px;
            border: none;
            border-radius: 6px;
            cursor: pointer;
            font-size: 0.8em;
            font-weight: 600;
            transition: all 0.2s ease;
        }
        .complete-btn {
            background: linear-gradient(135deg, #27ae60 0%, #2ecc71 100%);
            color: white;
        }
        .complete-btn:hover {
            background: linear-gradient(135deg, #229954 0%, #28b463 100%);
            transform: translateY(-1px);
        }
        .delete-btn {
            background: linear-gradient(135deg, #e74c3c 0%, #c0392b 100%);
            color: white;
        }
        .delete-btn:hover {
            background: linear-gradient(135deg, #cb4335 0%, #a93226 100%);
            transform: translateY(-1px);
        }
        .empty-column {
            text-align: center;
            color: #bdc3c7;
            font-style: italic;
            padding: 40px 20px;
            font-size: 1em;
        }
        .no-tasks {
            text-align: center;
            color: #7f8c8d;
            font-style: italic;
            padding: 60px 40px;
            font-size: 1.2em;
            background: #f8f9fa;
            border-radius: 12px;
            margin: 20px;
        }
        
        /* Responsive Design */
        @media (max-width: 768px) {
            .kanban-columns {
                flex-direction: column;
                gap: 15px;
            }
            .kanban-column {
                min-height: 300px;
            }
            .column-content {
                max-height: 400px;
            }
            .task-actions {
                flex-direction: column;
            }
            .action-btn {
                margin-bottom: 5px;
            }
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