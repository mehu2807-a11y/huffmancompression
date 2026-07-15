CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

SRCS := main.cpp HuffmanTree.cpp Compressor.cpp Decompressor.cpp FileHandler.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := huffman

.PHONY: all clean run demo

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

demo: all
	./$(TARGET) demo

clean:
	rm -f $(OBJS) $(TARGET) compressed.bin output.txt
