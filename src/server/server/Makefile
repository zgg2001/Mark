CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../../ -c
TARGET = m_server.o
OBJS = m_server.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/server/

clean:
	rm -rf $(TARGET)
