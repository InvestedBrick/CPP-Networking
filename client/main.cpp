#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include "../shared/grid.hpp"

Grid grid;

#define MAX_BUF_SIZE (GRID_WIDTH * GRID_HEIGHT)
#define FAILED -1
void log(const std::string &message) {
    std::cout << "[LOG] " << message << std::endl;
}
void warn(const std::string &message) {
    std::cerr << "[WARN] " << message << std::endl;
}
void fail(const std::string &message) {
    std::cerr << "[FAIL] " << message << std::endl;
    exit(EXIT_FAILURE);
}

int main(){
    // create client socket
    std::fstream file("connection_target.txt", std::ios::in);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return EXIT_FAILURE;
    }
    std::string line;
    std::getline(file, line);
    file.close();
    std::string ip = line.substr(0, line.find(':'));
    std::string port = line.substr(line.find(':') + 1);
    int PORT = std::stoi(port);
    const char* hostname = ip.c_str();

    // ger IP
    hostent* host = gethostbyname(hostname);
    if (!host) {
        std::cerr << "Failed to resolve hostname\n";
        return EXIT_FAILURE;
    }

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == FAILED) {
        fail("Failed to create socket");
    }else{
        log("Created socket successfully");
    }


    // connect the socket
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);
    if (connect(socketfd, (sockaddr*) &serv_addr,sizeof(serv_addr)) == FAILED){
        fail("Failed to connect to server");
    }else{
        log("Connected to server");
    }

    char buffer[MAX_BUF_SIZE];
    while(true){
        memset(buffer, 0, MAX_BUF_SIZE);
        std::string msg;
        std::cout << "Enter message: ";
        std::getline(std::cin, msg);
        if (msg == "exit") {
            break;
        }
        send(socketfd,msg.c_str(),msg.size(),0);

        int bytes_received = recv(socketfd, buffer, MAX_BUF_SIZE, 0);
        if (bytes_received == FAILED) {
            warn("Failed to receive message");
        }
        grid.decode_long(std::string(buffer,0,MAX_BUF_SIZE));
        std::cout << "Grid:" << std::endl;
        std::cout << grid.to_string() << std::endl;
    }
    close(socketfd);
    return 0;
}