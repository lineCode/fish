CC = g++
AR = ar
CFLAG = -g -O2 -fPIC -Wall

INCLUDE = -I. -I../3rd -I../3rd/bson -I../3rd/bson/src -I../3rd/protobuf-2.6.0/src
TARGET = ./libcore.a

OBJ = EventPoller.o \
	  EpollPoller.o \
	  TcpReader.o \
	  Network.o \
	  Reader.o \
	  Session.o \
	  WakeupSession.o \
	  LuaAcceptor.o \
	  LuaBson.o \
	  LuaConnector.o \
	  LuaMongo.o \
	  LuaReader.o \
	  LuaFish.o \
	  LuaServer.o \
	  LuaSocket.o \
	  LuaTimer.o \
	  MongoCursor.o \
	  MongoSession.o \
	  MongoBase.o \
	  Thread.o \
	  TimerManager.o \
	  Timestamp.o \
	  BufferHelper.o \
	  format.o \
	  MemoryStream.o \
	  MessageReader.o \
	  MessageWriter.o \
	  ZeroPack.o \
	  AppThread.o \
	  Logger.o \
	  ServerApp.o \


SRC = ./network/EventPoller.cpp ./network/EpollPoller.cpp ./network/TcpReader.cpp ./network/Network.cpp ./network/Reader.cpp ./network/Session.cpp ./network/WakeupSession.cpp \
./lualib/LuaAcceptor.cpp ./lualib/LuaBson.cpp ./lualib/LuaConnector.cpp ./lualib/LuaMongo.cpp ./lualib/LuaReader.cpp ./lualib/LuaFish.cpp ./lualib/LuaServer.cpp ./lualib/LuaSocket.cpp ./lualib/LuaTimer.cpp \
./mongo/MongoBase.cpp ./mongo/MongoCursor.cpp ./mongo/MongoSession.cpp \
./thread/Thread.cpp \
./time/TimerManager.cpp ./time/Timestamp.cpp \
./util/BufferHelper.cpp ./util/format.cc ./util/MemoryStream.cpp ./util/MessageReader.cpp ./util/MessageWriter.cpp ./util/ZeroPack.cpp \
./AppThread.cpp ./Logger.cpp ./ServerApp.cpp

all:$(TARGET)

$(OBJ):$(SRC)
	$(CC) $(CFLAG) $(INCLUDE) -c $(SRC)

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -f *.o $(TARGET)
