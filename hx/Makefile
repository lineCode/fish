AR = ar
CXXFLAG = -g -std=c++11 -fPIC -Wall -fno-omit-frame-pointer -fpermissive -fno-strict-aliasing -Wno-unused-local-typedefs 
CFLAG = -g -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs 

INCLUDE = -I. -I../3rd -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser
TARGET = libhx.a

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp $(dir)/*.cc $(dir)/*.c))

SRC += ../3rd/http-parser/http_parser.c ../3rd/luafilesystem/src/lfs.c


OBJ = $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp,%.o,$(SRC))))

all:$(TARGET)

%.o: %.cpp
	$(CC) $(CXXFLAG) $(INCLUDE) -c $< -o $@

%.o: %.cc
	$(CC) $(CXXFLAG) $(INCLUDE) -c $< -o $@

%.o: %.c
	gcc $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -f lualib/*.o
	rm -f lualib/json/*.o
	rm -f network/*.o
	rm -f time/*.o
	rm -f util/*.o
	rm -f thread/*.o
	rm -f logger/*.o
	rm -f ../3rd/http-parser/*.o
	rm -f ../3rd/luafilesystem/src/*.o
	rm -f *.o 
	rm -f $(TARGET)
