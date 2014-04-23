ifeq ($(OS), Windows_NT)
	CC = gcc
endif
CFLAGS = -Wall -pedantic -std=c90
EXECUTABLE=main
SOURCES=main.c parser.c checker.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f -- $(EXECUTABLE) $(EXECUTABLE).exe $(OBJECTS)
