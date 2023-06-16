#include <stdio.h>
#include <iostream>
#include "server.hpp"

int main() {
    yunying::Server server = yunying::Server();
    server.start();
    printf("Server started\n");
    std::string command;
    while (std::cin >> command, command != "quit") {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}