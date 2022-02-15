CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../../ -c
TARGET = m_plan.o m_user.o m_group.o
OBJS = m_plan.cpp m_user.cpp m_group.cpp

all:$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/server/

clean:
	rm -rf $(TARGET)
