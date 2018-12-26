CC = g++
AR = ar
CFLAG = -g -fPIC -Wall -fno-strict-aliasing

INCLUDE = -I. -I../3rd -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = ./libhx.a

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

all:$(TARGET)

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -f lualib/*.o
	rm -f network/*.o
	rm -f time/*.o
	rm -f util/*.o
	rm -f *.o 
	rm -f $(TARGET)