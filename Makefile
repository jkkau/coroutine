all:
	gcc main.c src/swap.s src/co.c -I src/ -g -o co

format:
	astyle --style=kr --indent=force-tab=4 ./*.c
	astyle --style=kr --indent=force-tab=4 ./src/*.c
	astyle --style=kr --indent=force-tab=4 ./src/*.h

clean:
	rm co