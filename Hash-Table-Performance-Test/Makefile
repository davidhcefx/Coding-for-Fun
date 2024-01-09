CFLAGS = -Wall -g -O2
TARGET = main

all: simple_ht.o main.o
	gcc -o $(TARGET) $^

clean:
	rm -rf *.o *.exe $(TARGET)

.PHONY: all clean
