#CC = g++ 
CFLAG = -g -std=c++11 -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs

#-fsanitize=address

INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser
TARGET = ./db

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))
STATIC_LIBS = ../bin/debug/libhx.a ../bin/debug/libev.a ../bin/debug/libtcmalloc.a ../bin/debug/liboolua.a  ../bin/debug/liblua.a

all:$(TARGET) 

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ) $(STATIC_LIBS)
	$(CC) -o $@ $^ -Wl,-E -lm -ldl -lrt -lpthread -lunwind -L/usr/lib64/mysql/ -lmysqlclient

clean:
	rm -f *.o $(TARGET)
