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
#include "../shared/grid.hpp"
#include "client_data.hpp"
Grid grid;
std::mutex mtx;

#define MAX_THREADS 12
#define MAX_BUF_SIZE (GRID_WIDTH * GRID_HEIGHT)
#define PORT 8080
#define CLOSE_PORT 4352
#define FAILED -1
int n_threads = 0;
bool close_server = false;


void increment_thread_count() {
    std::lock_guard<std::mutex> lock(mtx);
    n_threads++;
}
void decrement_thread_count() {
    std::lock_guard<std::mutex> lock(mtx);
    n_threads--;
}

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

int create_socket_fd(const int port, const char* ip){
    int socketfd = socket(AF_INET, SOCK_STREAM,0);
    if (socketfd == FAILED){
        fail("Failed to create socket");
    }else{
        log("Socket created");
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET; // IPv4
    hint.sin_addr.s_addr = INADDR_ANY;
    hint.sin_port = htons(port);
    inet_pton(AF_INET,ip,&hint.sin_addr); // bind to any address

    if (bind(socketfd, (sockaddr*)&hint, sizeof(hint)) == FAILED){
        fail("Failed to bind socket");
    }else{
        log("Socket bound");
    }

    return socketfd;
}
void handle_client(void* args){
    int client_socket = *reinterpret_cast<int*>(args);
    delete reinterpret_cast<int*>(args);
    // recieve messages
    client_data c_data;
    char instruc;
    char buf[MAX_BUF_SIZE];
    while(true){

        if (close_server){
            return;
        }
        int bytes_recieved = recv(client_socket, &instruc, 1, 0);
        if (bytes_recieved == FAILED){
            warn("Failed to receive message");
            continue;
        }else if (bytes_recieved == 0){
            log("Client disconnected");
            break;
        }
        c_data.parse_char(instruc);
        if (c_data.color_current_cell){
            {
                std::lock_guard<std::mutex> lock(mtx);

                grid.set_pixel(c_data.pos_x,c_data.pos_y,c_data.current_color);
            }
            c_data.color_current_cell = false;
        }
        // send grid
        std::string grid_str = grid.encode_long();
        grid_str[c_data.pos_y * GRID_WIDTH + c_data.pos_x] = 'X'; // set player cursor
        grid_str = grid.rle_encode(grid_str);
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Sending " << grid_str.length() << " bytes!" << std::endl;
        }
        memcpy(buf,grid_str.c_str(),grid_str.length());
        send(client_socket, buf, grid_str.length(), 0); // send the field
        
    } 
  
    // Close socket
    close(client_socket);
    decrement_thread_count();
}

void listener_thread(void* args){
    int socketfd = *(int*)args;
    while(true){

        sockaddr_in client;
        socklen_t client_size = sizeof(client);
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        
        int client_socket = FAILED;
        
        if (n_threads < MAX_THREADS){
            client_socket = accept(socketfd, (sockaddr*) &client, &client_size);
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        if (close_server){
            return;
        }
        if (client_socket == FAILED){
            warn("Failed to accept connection");
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
        std::thread client_handler(handle_client,new int(client_socket));
        client_handler.detach();
        increment_thread_count();
    }
    
}

void closing_thread(void* args) {
    //closing thread
    int closing_threadfd = create_socket_fd(CLOSE_PORT,"127.0.0.1");

    // listen
    if (listen(closing_threadfd, SOMAXCONN) == FAILED){
        fail("Failed to listen");
    }else{
        log("Closing thread listening on port " + std::to_string(CLOSE_PORT));
    }
    sockaddr_in client;
    socklen_t client_size = sizeof(client);
    while(true){

        if (accept(closing_threadfd,(sockaddr*) &client,&client_size) != FAILED){
            grid.save_canvas();
            close_server = true;
            log("Closed Server");
            close(closing_threadfd);
            close(*(int*)args);
            exit(EXIT_SUCCESS);
        }
    }
    
}

int main() {

    int socketfd = create_socket_fd(PORT,"0.0.0.0");
    
    // listen
    if (listen(socketfd, SOMAXCONN) == FAILED){
        fail("Failed to listen");
    }else{
        log("Listening on port " + std::to_string(PORT));
    }

    // create a listening thread
    std::thread listener(listener_thread, &socketfd);
    std::thread closer(closing_thread,&socketfd);

    closer.join();
    listener.join();

    return 0;
}