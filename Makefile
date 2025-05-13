all: build run

build:
	gcc main.c -o obscura -lraylib

run: build
	./obscura.exe

clean:
	rm -f obscura