all:
	gcc main.c swap.s -g -o co

format:
	astyle --style=kr --indent=force-tab=4 ./*.c

clean:
	rm co