CC = gcc
CFLAGS = -Wall -g -lreadline
TARGET = PMan
SOURCES = PMan.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)	

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
