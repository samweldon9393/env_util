CC = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors

example: env.a

env.a: env.o env.h
	ar rcs env.a env.o

env.o: env.h

clean:
	rm -f *.o *.a env_driver
