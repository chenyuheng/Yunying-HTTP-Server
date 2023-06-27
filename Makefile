CXX = g++ -std=c++20

CXX_FLAGS = -Wall -g -O2 -lpthread
LUA_FLAGS = -I"src/third_party/sol" -I"/usr/local/include" -I"/usr/local/bin" -llua

all:
	$(CXX) -o yunying src/*.cpp $(CXX_FLAGS) $(LUA_FLAGS)
clean:
	rm -f yunying
