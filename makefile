CCFLAGS=-Wall -Wextra -Werror -Iinclude

all: output main.o

bin1: 
	mkdir bin1

output: bin1/main.o
	g++ main.o -o output

bin1/main.o: main.cpp | bin1
	g++ -c main.cpp -std=c++11

clean:
	rm -rf bin1 output *.o

