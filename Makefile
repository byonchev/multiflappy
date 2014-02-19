CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-lwebsock
SOURCES=src/main.cpp src/Player.cpp src/Message.cpp src/FlappyServer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o bin/$@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*o bin/server
