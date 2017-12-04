CC = g++

part1:
	$(CC) -Wall -pthread -o part1 concurrencyPart1.cpp

part2:
	$(CC) -Wall -pthread -o part2 concurrencyPart2.cpp
	
all: part1 part2

clean:
	\rm *.o part1 part2;

tar:
	tar -cvjf concurrency.tar.bz2 concurrencyPart1.cpp concurrencyPart2.cpp Makefile goo instructions.txt
