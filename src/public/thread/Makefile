CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -c
TARGET = m_semaphore.o m_thread.o
OBJS = m_semaphore.cpp m_thread.cpp

all:$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp *.o ../../../build/public/

clean:
	rm -rf $(TARGET) 
