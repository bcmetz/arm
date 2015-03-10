CC=gcc
CFLAGS=
LDFLAGS= -lrt
SOURCES= $(wildcard mtr_src/*.c) \
			$(wildcard mtr_if/*.c ) \
			$(wildcard log/*.c)\
		   $(wildcard comm/*.c) 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=arm

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm $(OBJECTS) arm
