CC := gcc
CFLAGS := -I. -Wall -g

EXAMPLES := $(basename $(notdir $(wildcard example/*.c)))
TARGETS := $(addprefix bin/, $(EXAMPLES))
SRCS := $(wildcard co/*.c co/*.s) $(wildcard timer/*.c)

$(shell mkdir -p bin)

.PHONY: all clean

all: $(TARGETS)

bin/%: example/%.c $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test:
	./bin/basic_usage
	@echo "\n"
	./bin/sleep

memory_check:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./bin/basic_usage
	@echo "\n"
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./bin/sleep

format:
	astyle --style=kr --indent=force-tab=4 example/*.c co/*.c timer/*.c co/*.h timer/*.h

clean:
	rm -rf ./bin
	rm co/*.orig timer/*.orig example/*.orig