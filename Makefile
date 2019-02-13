BIN_PATH = ./bin/debug

LUA_STATICLIB := $(BIN_PATH)/liblua.a
LUA_LIB ?= liblua.a
LUA_INC ?= ./3rd/lua/src

OOLUA_LIB ?= liboolua.a
OOLUA_STATICLIB := $(BIN_PATH)/$(OOLUA_LIB)
OOLUA_INC ?= ./3rd/oolua/include
OOLUA_PATH ?= ./3rd/oolua
OOLUA_LIB_PATH ?= ./3rd/oolua/bin/Release

LIBEV_STATICLIB := $(BIN_PATH)/libev.a
LIBEV_LIB ?= libev.a
LIBEV_INC ?= ./3rd/libev

TCMALLOC_LIB ?= libtcmalloc_and_profiler.a
TCMALLOC_STATICLIB := $(BIN_PATH)/$(TCMALLOC_LIB)
TCMALLOC_INC ?= ./3rd/gperftools-2.7/src
TCMALLOC_PATH ?= ./3rd/gperftools-2.7

HX_STATICLIB := $(BIN_PATH)/libhx.a
HX_LIB ?= libhx.a
HX_INC ?= ./hx

FISH := $(BIN_PATH)/fish
FISH_INC ?= ./server

LOGGER := $(BIN_PATH)/logger
LOGGER_INC ?= ./logger

AGENT := $(BIN_PATH)/agent
AGENT_INC ?= ./agent

DB := $(BIN_PATH)/db
DB_INC ?= ./db

TEST := $(BIN_PATH)/test
TEST_INC ?= ./test

LIBS_DIR := $(BIN_PATH)

CCFLAG="CXX=g++ CC=gcc"

all: $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB) $(FISH) $(LOGGER) $(AGENT) $(DB) $(TEST)

$(LUA_STATICLIB) :
	cd $(LUA_INC)&& $(MAKE) linux
	mv $(LUA_INC)/$(LUA_LIB) $(LIBS_DIR) 

$(OOLUA_STATICLIB) :
	cd $(OOLUA_PATH) && $(MAKE) config=release
	mv $(OOLUA_LIB_PATH)/$(OOLUA_LIB) $(LIBS_DIR)/$(OOLUA_LIB)

$(LIBEV_STATICLIB) :
	cd $(LIBEV_INC) && ./configure && $(MAKE)
	mv $(LIBEV_INC)/.libs/libev.a $(LIBS_DIR)

$(TCMALLOC_STATICLIB) :
	cd $(TCMALLOC_PATH) && ./configure && $(MAKE)
	mv $(TCMALLOC_PATH)/.libs/$(TCMALLOC_LIB) $(LIBS_DIR)

$(HX_STATICLIB) :
	cd $(HX_INC) && $(MAKE) $(CCFLAG) 
	mv $(HX_INC)/$(HX_LIB) $(LIBS_DIR)
 
$(FISH) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB)
	cd $(FISH_INC) && $(MAKE) $(CCFLAG)
	mv $(FISH_INC)/fish $(LIBS_DIR)

$(AGENT) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB)
	cd $(AGENT_INC) && $(MAKE) $(CCFLAG) 
	mv $(AGENT_INC)/agent $(LIBS_DIR) 

$(LOGGER) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB)
	cd $(LOGGER_INC) && $(MAKE) $(CCFLAG) 
	mv $(LOGGER_INC)/logger $(LIBS_DIR) 

$(DB) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB)
	cd $(DB_INC) && $(MAKE) $(CCFLAG) 
	mv $(DB_INC)/db $(LIBS_DIR) 

$(TEST) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(TCMALLOC_STATICLIB) $(HX_STATICLIB)
	cd $(TEST_INC) && $(MAKE)
	mv $(TEST_INC)/test $(LIBS_DIR) 

leak :
	$(MAKE) $(ALL) CXX=clang CC=clang

release :
	$(MAKE) $(ALL) CXX=g++ CC=gcc config=release

test : $(TEST)

cleanall:
	make cleanhx
	rm -rf $(LIBS_DIR)/$(LUA_LIB) && cd $(LUA_INC) && make clean
	rm -rf $(LIBS_DIR)/$(OOLUA_LIB) && cd $(OOLUA_PATH) && make clean config=release
	rm -rf $(LIBS_DIR)/$(LIBEV_LIB) && cd $(LIBEV_INC) && make clean
	rm -rf $(LIBS_DIR)/$(TCMALLOC_LIB) && cd $(TCMALLOC_PATH) && make clean

cleanhx:
	make clean
	rm -rf $(LIBS_DIR)/$(HX_LIB) && cd $(HX_INC) && make clean

clean:
	rm -rf $(FISH) && cd $(FISH_INC) && make clean
	rm -rf $(AGENT) && cd $(AGENT_INC) && make clean
	rm -rf $(LOGGER) && cd $(LOGGER_INC) && make clean
	rm -rf $(DB) && cd $(DB_INC) && make clean
	rm -rf $(TEST) && cd $(TEST_INC) && make clean

cleantest:
	rm -rf $(TEST) $(TEST_INC)/*o
	
	
