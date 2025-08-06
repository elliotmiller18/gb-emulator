# Compiler and flags
CXX := clang++
CXXFLAGS := -O3 -Wall -Wextra -std=c++20 -I.

SDL_CXXFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

CXXFLAGS += $(SDL_CXXFLAGS)

# Source files
SRCS := $(shell find . -type f -name "*.cpp")
OBJS := $(SRCS:.cpp=.o)

# Output binary
TARGET := emulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDL_LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean