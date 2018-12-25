CC = g++
CFLAG = -g -fPIC -Wall

INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = ./fish
OBJ = main.o FishApp.o Bootstrap.o ClientSession.o MessageHandler.o FishMongo.o
SRC = main.cpp FishApp.cpp Bootstrap.cpp ClientSession.cpp MessageHandler.cpp FishMongo.cpp

all:$(TARGET)

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(CC) -o $@ $^ -L../libs -llua -lcore -lm -ldl -lrt -lpthread -lboost_thread -lboost_chrono -lboost_date_time -lboost_system

clean:
	rm -f *.o $(TARGET)