CC=gcc
CFLAGS=
LDFLAGS= -L/opt/local/lib -lzmq
SOURCES= $(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mac_test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
