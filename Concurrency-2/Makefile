CC = g++
concurrency : $(OBJS)
	$(CC) -pthread -o concurrency concurrency.cpp

clean:
	\rm *.o *~ concurrency

tar:
	tar cfv concurrency.tar concurrency.cpp