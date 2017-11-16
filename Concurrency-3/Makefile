CC = g++
concurrency : $(OBJS)
	$(CC) -Wall -pthread -o concurrency concurrency.cpp

clean:
	\rm *.o *~ concurrency

tar:
	tar -cvjf concurrency.tar.bz2 concurrency.cpp Makefile goo README.md
