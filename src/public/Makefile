all:
	mkdir -p ../../build/public
	cd database && make
	cd input && make
	cd log && make
	cd mail && make
	cd security && make
	cd thread && make
	cd time && make

clean:
	cd database && make clean
	cd input && make clean
	cd log && make clean
	cd mail && make clean
	cd security && make clean
	cd thread && make clean
	cd time && make clean
	mkdir -p ../../build/public
	rm -rf ../../build/public/*.o
