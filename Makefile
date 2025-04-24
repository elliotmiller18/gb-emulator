# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I.

# Source files
SRCS := *.cpp
OBJS := $(SRCS:.cpp=.o)

# Output binary
TARGET := emulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean