all: output file_queue.o

output: file_queue.o
	g++ file_queue.o -o output

file_queue.o: file_queue.cpp
	g++ -c file_queue.cpp -std=c++11

clean:
	rm *.o output

