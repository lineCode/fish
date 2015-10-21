CC = g++
AR = ar
CFLAG = -g -O2 -fPIC -Wall

INCLUDE = -I. -I../lib/core -I../lib/3rd -I../lib/3rd/lua/src -I../lib/3rd/bsoncpp -I../lib/3rd/bsoncpp/src
TARGET = ./fish
OBJ = main.o FishApp.o Bootstrap.o ClientSession.o MessageHandler.o FishMongo.o
SRC = main.cpp FishApp.cpp Bootstrap.cpp ClientSession.cpp MessageHandler.cpp FishMongo.cpp

all:$(TARGET)

$(OBJ):$(SRC)
	$(CC) $(CFLAG) $(INCLUDE) -c $(SRC)

$(TARGET):$(OBJ)
	$(CC) -o $@ $^ -L../libs -llua -lcore -lm -ldl -lrt -lpthread -lboost_thread -lboost_chrono -lboost_date_time -lboost_system

clean:
	rm -f *.o $(TARGET)