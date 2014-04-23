PROG=main
OBJS=main.o check.o parse.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(OBJS) -o $(PROG)

clean:
	rm -f -- $(PROG) $(OBJS)
