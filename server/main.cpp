#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <queue>

std::mutex mtx;

#define MAX_BUF_SIZE 4096
#define PORT 8080
#define FAILED -1


void log(const std::string &message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "[LOG] " << message << std::endl;
}

void fail(const std::string &message) {
    std::cerr << "[FAIL] " << message << std::endl;
    exit(EXIT_FAILURE);
}

void warn(const std::string &message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cerr << "[WARN] " << message << std::endl;
}

void handle_client(void* args){
    int client_socket = *(int*)args;
    // recieve messages

    char buf[MAX_BUF_SIZE];
    while(true){
        memset(buf,0, MAX_BUF_SIZE); // clear buffer
        int bytes_recieved = recv(client_socket, buf, MAX_BUF_SIZE, 0);
        if (bytes_recieved == FAILED){
            warn("Failed to receive message");
        }else if (bytes_recieved == 0){
            log("Client disconnected");
            break;
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Recieved: " << std::string(buf,0,bytes_recieved) << std::endl;
        }
        send(client_socket, buf, bytes_recieved + 1, 0); // echo back
    } 
  
    // Close socket
    close(client_socket);
    
}

void listener_thread(void* args){
    int socketfd = *(int*)args;
    while(true){

        sockaddr_in client;
        socklen_t client_size = sizeof(client);
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        
        int client_socket = accept(socketfd, (sockaddr*) &client, &client_size);
        
        if (client_socket == FAILED){
            fail("Failed to accept connection");
        }else{
            log("Client connected");
        }

        memset(host, 0, NI_MAXHOST); // clear out the host string
        memset(service, 0, NI_MAXSERV); // clear out the service string

        int result = getnameinfo((sockaddr*) & client, client_size, host,NI_MAXHOST, service, NI_MAXSERV, 0);

        if (result) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << host << " connected on " << service << std::endl;
            }
        }else{
            inet_ntop(AF_INET, &client.sin_addr,host, NI_MAXHOST);
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
            }
        }

        // create a thread to handle the client
        std::thread client_handler(handle_client,&client_socket);
        client_handler.detach();
    }
    
}

int main() {

    // create a socket

    int socketfd = socket(AF_INET, SOCK_STREAM,0);

    if (socketfd == FAILED){
        fail("Failed to create socket");
    }else{
        log("Socket created");
    }

    // bind the socket to IP and port
    sockaddr_in hint;
    hint.sin_family = AF_INET; // IPv4
    hint.sin_addr.s_addr = INADDR_ANY;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr); // bind to any address
    
    if (bind(socketfd, (sockaddr*)&hint, sizeof(hint)) == FAILED){
        fail("Failed to bind socket");
    }else{
        log("Socket bound");
    }
    
    // listen
    if (listen(socketfd, SOMAXCONN) == FAILED){
        fail("Failed to listen");
    }else{
        log("Listening on port " + std::to_string(PORT));
    }

    // create a listening thread
    std::thread listener(listener_thread, &socketfd);
    listener.join();

    close(socketfd);
    return 0;
}