CC=g++
ARGS=--std=c++11
INCLUDE=include/
all: FrameAllocator.o
FrameAllocator.o: src/FrameAllocator.cpp include/FrameAllocator.hpp
	$(CC) -c src/FrameAllocator.cpp -o bin/FrameAllocator.o $(ARGS) -I$(INCLUDE)
test: bin/FrameAllocator.o test/FrameAllocatorTest.cpp
	$(CC) test/FrameAllocatorTest.cpp bin/FrameAllocator.o -o bin/FrameAllocatorTest -I$(INCLUDE)
clean:
	rm bin/*
