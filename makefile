CCFLAGS=-Wall -Wextra -Werror -Iinclude

all: output main.o

bin: 
	mkdir bin

output: bin/main.o
	g++ main.o -o output

bin/main.o: src/main.cpp | bin
	g++ -c src/main.cpp -std=c++11

clean:
	rm -rf bin output *.o

