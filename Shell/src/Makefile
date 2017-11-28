CC=gcc
CFLAGS=-c -g -Wall -Wextra
LDFLAGS=-g -Wall -Wextra

SOURCES=input.c output.c command.c shell.c proc.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=shell


.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

.PHONY: run
run:
	./shell
