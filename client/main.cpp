#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
//Stuff for changing therminal mode
#include <termios.h>
#include <fcntl.h>

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


void set_non_blocking(bool enable) {
    static struct termios oldt, newt;

    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);           // get current terminal attributes
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);         // disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // apply settings

        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // make read non-blocking
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // restore terminal settings
        fcntl(STDIN_FILENO, F_SETFL, 0);          // restore blocking
    }
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
    std::string msg = "s";
    char ch;
    while(true){
        system("clear");
        std::cout << "The Grid; Use WASD to move, 'q' to quit and SPACE to color the current cell" << std::endl;
        std::cout << "To change color, press the following to choose: " << std::endl;
        std::cout << color_to_string(ANSI_COLOR_BLACK) <<"0 = BLACK"<< color_to_string(ANSI_COLOR_RESET) << " 1 = WHITE" << color_to_string(ANSI_COLOR_RED)
        << " 2 = RED"<< color_to_string(ANSI_COLOR_GREEN)<<" 3 = GREEN"<< color_to_string(ANSI_COLOR_YELLOW)<<" 4 = YELLOW"<< color_to_string(ANSI_COLOR_BLUE)
        <<" 5 = BLUE"<< color_to_string(ANSI_COLOR_MAGENTA)<<" 6 = MAGENTA"<< color_to_string(ANSI_COLOR_CYAN)<<" 7 = CYAN " << color_to_string(ANSI_COLOR_RESET) << std::endl;
        std::cout << grid.to_string() << std::endl; // ~ 2ms
        
        std::flush(std::cout);

        set_non_blocking(true);

        while (true){
            if(read(STDIN_FILENO,&ch,1) > 0){
                //std::cout << "You pressed: " << ch << std::endl; 
                break;
            }
            usleep(100);
        }
        set_non_blocking(false);

        if (ch == 'q') {
            break;
        }

        send(socketfd,&ch,1,0);

        memset(buffer, 0, MAX_BUF_SIZE);
        int bytes_received = recv(socketfd, buffer, MAX_BUF_SIZE, 0);
        if (bytes_received == FAILED) {
            warn("Failed to receive message");
        }

        grid.decode_long(grid.rle_decode_efficient(std::string(buffer,0,bytes_received)));
        
        
    }
    close(socketfd);
    return 0;
}