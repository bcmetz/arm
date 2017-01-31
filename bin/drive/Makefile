CC=g++
CFLAGS= -O0 -D _OS_X_ -D__linux__ -I /opt/local/include
LDFLAGS= -L/opt/local/lib -lzmq

SOURCES= $(wildcard *.c)\
				 $(wildcard *.cpp)

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=drive

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm *.o $(EXECUTABLE)
