CC = g++
CFLAG = -g -fPIC -Wall

INCLUDE = -I. -I../3rd -I../hx -I../3rd/libev -I../3rd/lua/src -I../3rd/oolua/include -I../3rd/rapidjson
TARGET = ./fish

DIRS = $(shell find . -maxdepth 3 -type d)
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(patsubst %.cpp, %.o, $(SRC))

all:$(TARGET)

%.o: %.cpp
	$(CC) $(CFLAG) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJ)
	$(CC) -o $@ $^ -L../libs -llua -lcore -lm -ldl -lrt -lpthread -lboost_thread -lboost_chrono -lboost_date_time -lboost_system

clean:
	rm -f *.o $(TARGET)