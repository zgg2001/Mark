CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g -I ../../ -c
TARGET = m_db_mgr.o
OBJS = m_db_mgr.cpp

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)
	cp $(TARGET) ../../../build/server/

clean:
	rm -rf $(TARGET)
