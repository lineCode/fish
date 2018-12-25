CC = g++
AR = ar
CFLAG = -g -fPIC -Wall -fno-strict-aliasing

INCLUDE = -I. -I../3rd -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = ./libhx.a

OBJ = ./network/EventPoller.o \
	  ./network/TcpReader.o \
	  ./network/Network.o \
	  ./network/Reader.o \
	  ./network/Channel.o \
	  ./thread/Thread.o \
	  ./time/Timestamp.o \
	  ./util/format.o \
	  ./util/MemoryStream.o \
	  ./util/MessageReader.o \
	  ./util/MessageWriter.o \
	  ./util/ZeroPack.o \
	  ./AppThread.o \
	  ./Logger.o \
	  ./ServerApp.o \


SRC = ./network/EventPoller.cpp ./network/TcpReader.cpp ./network/Network.cpp ./network/Reader.cpp ./network/Channel.cpp \
./lualib/LuaFish.cpp ./thread/Thread.cpp ./time/Timestamp.cpp \
./util/format.cc ./util/MemoryStream.cpp ./util/MessageReader.cpp ./util/MessageWriter.cpp ./util/ZeroPack.cpp \
./AppThread.cpp ./Logger.cpp ./ServerApp.cpp

all:$(TARGET)

# $(OBJ):$(SRC)
# 	$(CC) $(CFLAG) $(INCLUDE) -c $(SRC)
# 	
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