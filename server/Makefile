CFLAG = -g -std=c++11  -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-local-typedefs -Wno-unused-variable 

INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser
TARGET = ./fish

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

STATIC_LIBS = ../bin/debug/libhx.a ../bin/debug/libev.a  ../bin/debug/libtcmalloc.a ../bin/debug/liboolua.a  ../bin/debug/liblua.a

all:$(TARGET) 

%.o: %.cpp 
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@ 

$(TARGET):$(OBJ) $(STATIC_LIBS)
	$(CC) -o $@ $^ -lm -ldl -lrt -lpthread -lstdc++ 

clean:
	rm -f *.o $(TARGET)
