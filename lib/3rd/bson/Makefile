CC = g++
AR = ar
CFLAG = -g -Wall

INCLUDE = -I. -I./src
TARGET = ./libbson.a

OBJ = bsonobj.o \
	  oid.o \
	  json.o \
	  base64.o \
	  nonce.o \
	  md5.o \


SRC = ./src/bsonobj.cpp ./src/oid.cpp ./src/util/json.cpp ./lib/base64.cpp \
./lib/nonce.cpp ./lib/md5.c \

all:$(TARGET)

$(OBJ):$(SRC)
	$(CC) $(CFLAG) $(INCLUDE) -c $(SRC)

$(TARGET):$(OBJ)
	$(AR) cq $@ $^

clean:
	rm -f *.o $(TARGET)
