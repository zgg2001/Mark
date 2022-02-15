CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -c
TARGET = m_timer.o
OBJS = m_timer.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/public/

clean:
	rm -rf $(TARGET)
