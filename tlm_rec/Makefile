CC=gcc
CFLAGS=
LDFLAGS= -lrt
SOURCES= $(wildcard ../mtr_clnt/*.c) \
			$(wildcard ../log/*.c)\
			$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tlm_rec

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
