#CFLAGS     = -Wall -pedantic -ggdb -c90
CFLAGS     = -Wall -pedantic -ggdb
EXECUTABLE = main
SOURCES    = $(wildcard ./*.c)
OBJECTS    = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f -- $(EXECUTABLE) $(OBJECTS)
