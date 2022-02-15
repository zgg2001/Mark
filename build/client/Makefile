CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g

TARGET = mark_client
OBJS = mark_client.o \
	   m_client.o \
	   m_recv_node.o \
	   m_send_node.o \
	   ../public/log.o \
	   ../public/m_input.o \
	   ../public/m_mail.o \
	   ../public/m_noecho.o \
	   ../public/m_semaphore.o \
	   ../public/m_thread.o \
	   ../public/m_timer.o


$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread -lcrypto -lnice
	cp $(TARGET) ../../bin/

clean:
	rm -rf $(TARGET) ../../bin/$(TARGET)
