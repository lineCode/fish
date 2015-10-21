CC = g++
CFLAGS = -O2 -fPIC -Wall
LUADIR = ../lua/src
TARGET = profiler.so

.PHONY : all clean

all : $(TARGET)

$(TARGET) : clocks.cpp core_profiler.cpp function_meter.cpp lua50_profiler.cpp stack.cpp
	$(CC) $(CFLAGS) -shared -o $@ -I$(LUADIR) $^

clean :
	rm -f $(TARGET)
