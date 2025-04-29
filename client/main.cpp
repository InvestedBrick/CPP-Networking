#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 4096
#define FAILED -1
#define PORT 8080
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
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == FAILED) {
        fail("Failed to create socket");
    }else{
        log("Created socket successfully");
    }

    // connect the socket
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);
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
        std::cout << "Server: " << std::string(buffer,0,bytes_received) << std::endl;
    }
    close(socketfd);
    return 0;
}