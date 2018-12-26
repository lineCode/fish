CC = g++
CFLAG = -g -std=c++11 -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs


INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = ./fish

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

all:$(TARGET) 

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ) ../bin/debug/liblua.a ../bin/debug/libhx.a ../bin/debug/liboolua.a ../bin/debug/libev.a
	$(CC) -o $@ $^ -L../bin/debug -llua -lm -ldl -lrt -lpthread 

clean:
	rm -f *.o $(TARGET)
