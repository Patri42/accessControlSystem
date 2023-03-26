PROG=main.exe
CC=gcc
CFLAGS=-g -Wall -Werror -std=c99
DEPS = 
OBJ = main.o controller.o
 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
 
$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)