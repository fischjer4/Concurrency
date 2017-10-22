OBJS = concurrency.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
concurrency : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o concurrency

concurrency.o : concurrency.cpp
	$(CC) $(CFLAGS) concurrency.cpp

clean:
	\rm *.o *~ concurrency

tar:
	tar cfv concurrency.tar concurrency.cpp
	
