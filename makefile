#CFLAGS     = -Wall -pedantic -ggdb
CFLAGS     = -pedantic
EXECUTABLE = myFind
SOURCES    = $(wildcard ./*.c)
OBJECTS    = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f -- $(EXECUTABLE) $(OBJECTS)
