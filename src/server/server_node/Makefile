CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../../ -c
TARGET = m_group_node.o m_login_node.o m_task_node.o
OBJS = m_group_node.cpp m_login_node.cpp m_task_node.cpp

all:$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/server/

clean:
	rm -rf $(TARGET)
