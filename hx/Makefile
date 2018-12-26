CC = g++
AR = ar
CFLAG = -g -std=c++11 -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs

INCLUDE = -I. -I../3rd -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = libhx.a

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp $(dir)/*.cc))
SRC += ../3rd/http-parser/http_parser.c

OBJ = $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp,%.o,$(SRC))))

all:$(TARGET)

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

%.o: %.cc
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -f lualib/*.o
	rm -f network/*.o
	rm -f time/*.o
	rm -f util/*.o
	rm -f thread/*.o
	rm -f ../3rd/http-parser/http_parser.o
	rm -f *.o 
	rm -f $(TARGET)
