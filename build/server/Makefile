CXX = g++
CFLAGS = -std=c++17 -O2 -Wall -g

TARGET = mark_server
OBJS = mark_server.o \
	   m_client_node.o \
	   m_db_mgr.o \
	   m_group.o \
	   m_group_node.o \
	   m_login_node.o \
	   m_plan.o \
	   m_server.o \
	   m_task_node.o \
	   m_user.o \
	   ../public/log.o \
	   ../public/m_db.o \
	   ../public/m_input.o \
	   ../public/m_mail.o \
	   ../public/m_noecho.o \
	   ../public/m_semaphore.o \
	   ../public/m_thread.o \
	   ../public/m_timer.o


$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread `mysql_config --cflags --libs`
	cp $(TARGET) ../../bin/

clean:
	rm -rf $(TARGET) ../../bin/$(TARGET)
