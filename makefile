CC = g++

CFLAGS = -Wall -g

LDFLAGS = 

TARGET = simple

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(LDFLAGS) main.o -o $(TARGET)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o $(TARGET)
