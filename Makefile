MAKEFLAGS += --no-print-directory

all:
	cd build && make

clean:
	cd build && make clean
