CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../../ -c
TARGET = m_recv_node.o m_send_node.o
OBJS = m_recv_node.cpp m_send_node.cpp

all:$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/client/

clean:
	rm -rf $(TARGET)
