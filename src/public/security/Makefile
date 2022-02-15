CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -c
TARGET = m_noecho.o
OBJS = m_noecho.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/public/

clean:
	rm -rf $(TARGET)
