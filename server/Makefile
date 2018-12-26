CC = g++
CFLAG = -g -std=c++11 -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs 


INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser
TARGET = ./fish

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

all:$(TARGET) 

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ) 
	$(CC) -o $@ $^ -Wl,-E -L../bin/debug -llua -lhx -loolua -lev -lm -ldl -lrt -lpthread 

clean:
	rm -f *.o $(TARGET)
