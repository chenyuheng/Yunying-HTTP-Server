all:
	g++ -o yunying src/*.cpp -std=c++20 -lpthread
clean:
	rm -f yunying
