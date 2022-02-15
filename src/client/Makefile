CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../ -c
TARGET = mark_client.o
OBJS = mark_client.cpp

all:
	mkdir -p ../../build/client/ 
	cd client && make
	cd client_node && make
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../build/client/

clean:
	cd client && make clean
	cd client_node && make clean
	mkdir -p ../../build/client/ 
	rm -rf $(TARGET) ../../build/client/*.o ../../build/client/$(TARGET)
