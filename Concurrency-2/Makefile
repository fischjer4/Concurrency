TARGETS = concurrency
CC_C = $(CROSS_TOOL)gcc 
CFLAGS = -Wall -pthread

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	$(RM) concurrency
	rm -rf concurrency.dSYM
