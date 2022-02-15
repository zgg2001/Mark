CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../ -c
TARGET = mark_server.o
OBJS = mark_server.cpp

all:
	mkdir -p ../../build/server/
	cd client_node && make
	cd data && make
	cd database && make
	cd server && make
	cd server_node && make
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../build/server/

clean:
	cd client_node && make clean
	cd data && make clean
	cd database && make clean
	cd server && make clean
	cd server_node && make clean
	mkdir -p ../../build/server/ 
	rm -rf $(TARGET) ../../build/server/*.o ../../build/server/$(TARGET)
