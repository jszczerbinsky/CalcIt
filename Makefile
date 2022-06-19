all:
	mkdir -p build
	gcc src/*.c -o build/calcIt

install:
	cp build/calcIt /usr/bin/
