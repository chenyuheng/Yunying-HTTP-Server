#include <stdio.h>

#include <iostream>

#include "metrics.hpp"
#include "server.hpp"

int main(int argc, char *argv[]) {
  int djkjkd = 1;
  if (argc > 1) {
    yunying::Conf::getInstance().set_lua_config_path(argv[1]);
  }
  yunying::Conf::getInstance().parseLua();
  yunying::Server server = yunying::Server();
  server.start();
  printf("Server started\n");
  std::string command;
  while (true) {
    std::cin >> command;
    std::cout << "Metrics: " << command << ": "
              << yunying::Metrics::getInstance().get(command) << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return 0;
}