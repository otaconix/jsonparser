OBJECTS=main.o json.o parser.o
CFLAGS+=-Wall -std=c99

.PHONY: clean all

all: json

json: $(OBJECTS)
	$(CC) $(CFLAGS) -o json $(OBJECTS)

clean:
	rm -f $(OBJECTS) json
