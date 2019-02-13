
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
	else
		CXXFLAG = -g -std=c++11 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-local-typedefs -Wno-unused-variable 
	endif
endif

ifeq ($(config),release)
	ifeq ($(CXX),clang)
		CXXFLAG = -g -std=c++11 -O3 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-variable -fsanitize=address
	else

		CXXFLAG = -g -std=c++11 -O3 -fPIC -Wall -fno-omit-frame-pointer -fno-strict-aliasing -Wno-unused-local-typedefs -Wno-unused-variable 
	endif
endif

INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson -I../3rd/http-parser
TARGET = ./db

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

STATIC_LIBS = ../bin/debug/libhx.a ../bin/debug/libev.a ../bin/debug/libtcmalloc_and_profiler.a ../bin/debug/liboolua.a  ../bin/debug/liblua.a

all:$(TARGET) 

%.o: %.cpp 
	$(CXX) $(CXXFLAG) $(INCLUDE) -c $< -o $@ 

$(TARGET):$(OBJ) $(STATIC_LIBS)
	$(CXX) -o $@ $^ -lm -ldl -lrt -lpthread -lstdc++ -lunwind -L/usr/local/mysql/lib -lmysqlclient

clean:
	rm -rf $(OBJ) $(TARGET)
