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

format:
	astyle --style=kr --indent=force-tab=4 example/*.c co/*.c timer/*.c co/*.h timer/*.h

clean:
	rm -rf ./bin
	rm co/*.orig timer/*.orig example/*.orig