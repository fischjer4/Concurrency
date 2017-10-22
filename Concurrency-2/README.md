The Dining Philosophers Problem was proposed by Dijkstra in 1965. It appears in a number of variations, but the standard features are a table with five plates, five forks (or chopsticks), and a big bowl of spaghetti. Five philosophers, who represent interacting threads, come to the table and execute the following loop:
```
	while True:
		think()
		get_forks()
		eat()
		put_forks()
```
Assuming that the philosophers know how to think() and eat(), write a solution to this problem

* There are several synchronization constraints that we need to enforce to make this system work correctly:
* A single fork can only be held by a single philosopher
* It must be impossible for deadlock to occur
* Starvation is disallowed
* More than a single philosopher must be capable of eating simultaneously
* All must be alive at program termination.

Write concurrent code which implements a solution which satisfies the above description. A few important details:

* Each bowl of noodles is infinite.
* Eating takes a random amount of time in the range of 2-9 seconds.
* Thinking takes a random amount of time in the range of 1-20 seconds.
* You must have output that makes sense, and shows the correctness of your solution.
	* This includes the status of all forks at all times.
	* The status of each philosopher, as well.