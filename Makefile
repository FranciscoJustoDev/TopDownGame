CC = gcc
CFLAGS = -lSDL2

SRCS = *.c

OUT = prog

make: build run clean

build:
	$(CC) $(SRCS) $(CFLAGS) -o $(OUT)

run:
	./$(OUT)

clean:
	rm $(OUT)
