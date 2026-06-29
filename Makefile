CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude

SRC = src/RingBuffer.cpp src/MockUart.cpp src/BufferedUart.cpp tests/main.cpp
OUT = oop_uart

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT) oop_uart.exe