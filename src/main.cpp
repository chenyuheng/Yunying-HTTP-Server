#include <stdio.h>
#include <iostream>
#include "server.hpp"

int main(int argc, char *argv[]) {
    yunying::Conf conf = yunying::Conf();
    if (argc > 1) {
        uint16_t port = atoi(argv[1]);
        if (port != 0) {
            conf.set_port(port);
        }
    }
    yunying::Server server = yunying::Server(conf);
    server.start();
    printf("Server started\n");
    std::string command;
    while (std::cin >> command, command != "quit") {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}