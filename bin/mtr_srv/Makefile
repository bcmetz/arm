CC=gcc
CFLAGS= 
LDFLAGS= -lrt -lzmq
SOURCES= $(wildcard *.c) \
			$(wildcard ../../mtr_if/*.c ) \
			$(wildcard ../../log/*.c)\
		   $(wildcard ../../comm/*.c) 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mtr_srv

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
