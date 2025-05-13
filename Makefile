# all: build run

build: build_obscura build_pong

build_obscura:
	gcc main.c -o obscura -lraylib

build_pong:
	gcc pong.c -o pong -lraylib

# run: build
# 	./obscura.exe

# run_pong: build_pong
# 	./pong.exe

clean:
	rm -f obscura pong