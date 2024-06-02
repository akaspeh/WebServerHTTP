#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <functional>
#include <string>


#pragma comment(lib, "Ws2_32.lib")


class HTTPServer {
private:
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr;
    int port;

public:
    HTTPServer(int port) : port(port) {}

    bool initialize() {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed.\n";
            return false;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed.\n";
            WSACleanup();
            return false;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);
        if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed.\n";
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed.\n";
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        return true;
    }

    std::string readHtmlFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.find(suffix, str.size() - suffix.size()) != std::string::npos;
    }


    std::string getContentType(const std::string& path) {
        if (endsWith(path, ".html")) return "text/html";
        if (endsWith(path, ".css")) return "text/css";
        if (endsWith(path, ".js")) return "application/javascript";
        return "text/plain";
    }


    void sendResponse(SOCKET clientSocket, const std::string& response) {
        send(clientSocket, response.c_str(), response.size(), 0);
    }


    void handleRequest(SOCKET clientSocket) {
        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::istringstream request(buffer);
            std::string method, path, version;
            request >> method >> path >> version;

            if (path == "/") {
                path = "/index.html";
            }

            std::string filesPath = "C:/Users/PC/CLionProjects/WebServerHTTP" + path;

            if (method == "GET") {
                std::string content = readHtmlFile(filesPath);
                std::string contentType = getContentType(filesPath);

                if (!content.empty()) {
                    std::string httpResponse =
                            "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\nContent-Length: " +
                            std::to_string(content.size()) + "\r\n\r\n" + content;
                    sendResponse(clientSocket, httpResponse);
                } else {
                    std::string httpResponse = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
                    sendResponse(clientSocket, httpResponse);
                }
            }
        }
    }

    void acceptConnections() {
        while (true) {
            // Accept connection
            SOCKET clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed.\n";
                continue;
            }


            std::thread(&HTTPServer:: handleRequest, this, clientSocket).detach();
            std::cout << "Accepted Client: " << clientSocket << "\n";
        }
    }

    ~HTTPServer() {
        // Clean up
        closesocket(serverSocket);
        WSACleanup();
    }
};

int main() {
    HTTPServer server(8080);
    if (server.initialize()) {
        std::cout << "Server initialized.\n";
        server.acceptConnections();
    } else {
        std::cerr << "Server initialization failed.\n";
    }
    return 0;
}
