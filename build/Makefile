MAKEFLAGS += --no-print-directory

all:
	mkdir -p ../bin
	cd ../src/ && make
	cd server && make
	cd client && make

clean:
	cd ../src/ && make clean
	cd server && make clean
	cd client && make clean
