CC=g++
CFLAGS = -O3     # optimize code
DFLAGS =         # common defines

DEPS = ulmann.h
OBJ = ulmann_seq.o ulmann.o
PROG = ulmann_seq

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -g -o $@ $<

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -g -o $@ $^

.PHONY: clean
clean:
	rm -f *.o $(PROG)



