**Classic Producer Consumer Problem**

There are several synchronization constraints that we need to enforce to make this system work correctly:
* While an item is being added to or removed from the buffer, the buffer is in an inconsistent state. Therefore, threads must have exclusive access to the buffer.
* If a consumer thread arrives while the buffer is empty, it blocks until a producer adds a new item.
* If a producer thread has an item to put in the buffer while the buffer is full, it blocks until a consumer removes an item.

Write C code with pthreads which implements a solution which satisfies the above description. A few important details:
* The item in the buffer should be a struct with two numbers in it.
	* The first value is just a number. The consumer will print out this value as part of its consumption.
	* The second value is a random waiting period between 2 and 9 seconds, which the consumer will sleep for prior to printing the other value. This is the "work" done to consume the item.
	* Both of these values should be created using the rdrand x86 ASM instruction on systems that support it, and using the Mersenne Twister on systems that don't support rdrand. It is your responsibility to learn how to do this, how to include it in your code, and how to condition the value such that it is within the range you want. Being able to work with x86 ASM is actually a necessary skill in this class. Hint: os-class does not support rdrand. Your laptop likely does.
* Your producer should also wait a random amount of time (in the range of 3-7 seconds) before "producing" a new item.
* Your buffer in this case can hold 32 items. It must be implicitly shared between the threads.
* Use whatever synchronization construct you feel is appropriate.
