CC = g++
CFLAGS = -O2 -fPIC -Wall
LUADIR = ../lua/src
TARGET = serialize.so

.PHONY : all clean

all : $(TARGET)

$(TARGET) : lua-seri.cpp
	$(CC) $(CFLAGS) -shared -o $@ -I$(LUADIR) $^

clean :
	rm -f $(TARGET)
