CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -c
TARGET = log.o
OBJS = log.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/public/

clean:
	rm -rf $(TARGET)
