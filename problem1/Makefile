GENERATES = main.out
TRASH = *.o
CC = g++
CPPFLAGS = -fsanitize=address,undefined,signed-integer-overflow,pointer-compare,pointer-subtract,leak,bounds,pointer-overflow -O2 -Wall -Wextra -Wpedantic -std=c++23 -lm


.PHONY: all

all: main.out

main.out: main.o shared_pointer.o logger.o formater.o
	$(CC) $(CPPFLAGS) -o $@ $^

main.o: main.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $^

shared_pointer.o: shared_pointer.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $^

logger.o: logger.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $^

formater.o: formater.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $^



trashclean:
	rm $(TRASH)

distclean: clean
	rm $(GENERATES)

clean: trashclean distclean
