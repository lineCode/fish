ifndef config
  config=debug
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifeq ($(config),debug)
	ifeq ($(CXX),clang)
		CXXFLAG = -g -std=c++11 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-variable -fsanitize=address
		CFLAG = -g -fPIC -Wall -fno-strict-aliasing -fsanitize=address
	else
		CXXFLAG = -g -std=c++11 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-local-typedefs -Wno-unused-variable 
		CFLAG = -g -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs 
	endif
endif

ifeq ($(config),release)
	ifeq ($(CXX),clang)
		CXXFLAG = -g -std=c++11 -O3 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-variable -fsanitize=address
		CFLAG = -g -O3 -fPIC -Wall -fno-strict-aliasing -fsanitize=address
	else

		CXXFLAG = -g -std=c++11 -O3 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-local-typedefs -Wno-unused-variable 
		CFLAG = -g -O3 -fPIC -Wall -fno-strict-aliasing -Wno-unused-local-typedefs 
	endif
endif

AR = ar

INCLUDE = -I. -I../3rd -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser -I../3rd/klib -I../3rd/gperftools-2.7/src/gperftools
TARGET = libhx.a

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp $(dir)/*.cc $(dir)/*.c))
SRC += ../3rd/http-parser/http_parser.c ../3rd/luafilesystem/src/lfs.c

OBJ = $(patsubst %.c, %.o, $(patsubst %.cc, %.o, $(patsubst %.cpp,%.o,$(filter-out ./util/pathfinder/wp/pathfinder.c,$(SRC)))))

all:$(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAG) $(INCLUDE) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAG) $(INCLUDE) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -rf $(OBJ)
	rm -rf $(TARGET)
