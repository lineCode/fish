CC = g++
CFLAGS = -O2 -fPIC -Wall
LUADIR = ../lua/src
TARGET = bson.so

.PHONY : all clean

all : $(TARGET)

$(TARGET) : lua-bson.cpp
	$(CC) $(CFLAGS) -shared -o $@ -I$(LUADIR) $^

clean :
	rm -f $(TARGET)
