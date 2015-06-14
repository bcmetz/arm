CC=gcc
CFLAGS=
LDFLAGS= -lrt -lzmq
SOURCES= $(wildcard ../../mtr_clnt/*.c) \
	 $(wildcard ../../log/*.c)\
	 $(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=clnt_test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
