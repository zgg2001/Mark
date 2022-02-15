CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -c
TARGET = m_db.o
OBJS = m_db.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/public/ 

clean:
	rm -rf $(TARGET)
