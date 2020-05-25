PWD=$(shell pwd)
SRC=$(shell find ./src | grep cpp)

.FORCE:
nython: .FORCE
	docker run --rm -it -v "${PWD}":/usr/src/nython -w /usr/src/nython gcc make compile run

compile: ${SRC}
	g++ -o nython -Iinclude -std=c++20 ${SRC}

run:
	./nython