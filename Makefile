CXX=g++ -std=c++17

lib:
	mkdir lib
bin:
	mkdir bin

main.o : lib
	$(CXX) -c -o lib/main.o src/main.cpp

build: bin main.o 
	$(CXX) -o bin/app  lib/main.o -pthread

run: build
	bin/app
clean:
	rm -rf lib bin