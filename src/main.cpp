#include <stdio.h>
#include <iostream>
#include "server.hpp"
#include "metrics.hpp"

int main(int argc, char *argv[]) {
    if (argc > 1) {
        uint16_t port = atoi(argv[1]);
        if (port != 0) {
            yunying::Conf::getInstance().set_port(port);
        }
    }
    if (argc > 2) {
        std::string root_dir = argv[2];
        yunying::Conf::getInstance().set_root_dir(root_dir);
    }
    yunying::Server server = yunying::Server();
    server.start();
    printf("Server started\n");
    std::string command;
    while (true) {
        std::cin >> command;
        std::cout << "Metrics: " << command << ": " << yunying::Metrics::getInstance().get(command) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}