BIN_PATH = ./bin/debug

LUA_STATICLIB := $(BIN_PATH)/liblua.a
LUA_LIB ?= liblua.a
LUA_INC ?= ./3rd/lua/src

OOLUA_STATICLIB := $(BIN_PATH)/liboolua.a
OOLUA_LIB ?= liboolua.a
OOLUA_INC ?= ./3rd/oolua/include
OOLUA_PATH ?= ./3rd/oolua
OOLUA_LIB_PATH ?= ./3rd/oolua/bin/Debug

LIBEV_STATICLIB := $(BIN_PATH)/libev.a
LIBEV_LIB ?= libev.a
LIBEV_INC ?= ./3rd/libev

HX_STATICLIB := $(BIN_PATH)/libhx.a
HX_LIB ?= libhx.a
HX_INC ?= ./hx

FISH := $(BIN_PATH)/fish
FISH_INC ?= ./server


LIBS_DIR := $(BIN_PATH)


all: $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(HX_STATICLIB) $(FISH)

$(LUA_STATICLIB) :
	cd $(LUA_INC)&& $(MAKE) linux
	mv $(LUA_INC)/$(LUA_LIB) $(LIBS_DIR) 
	mv $(LUA_INC)/liblua.so $(LIBS_DIR) 

$(OOLUA_STATICLIB) :
	cd $(OOLUA_PATH) && $(MAKE)
	mv $(OOLUA_LIB_PATH)/liboolua_d.a $(LIBS_DIR)/$(OOLUA_LIB)

$(LIBEV_STATICLIB) :
	cd $(LIBEV_INC) && ./configure && $(MAKE)
	mv $(LIBEV_INC)/.libs/libev.a $(LIBS_DIR)

$(HX_STATICLIB) :
	cd $(HX_INC) && $(MAKE)
	mv $(HX_INC)/$(HX_LIB) $(LIBS_DIR)
 
$(FISH) : $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(HX_STATICLIB)
	cd $(FISH_INC) && $(MAKE)
	mv $(FISH_INC)/fish $(LIBS_DIR) 
 

cleanall:
	rm -rf $(FISH) $(FISH_INC)/*o
	rm -rf $(LIBS_DIR)/$(HX_LIB) && cd $(HX_INC) && make clean
	rm -rf $(LIBS_DIR)/$(LUA_LIB) $(LIBS_DIR)/liblua.so && cd $(LUA_INC) && make clean
	rm -rf $(LIBS_DIR)/$(OOLUA_LIB) && cd $(OOLUA_PATH) && make clean
	rm -rf $(LIBS_DIR)/$(LIBEV_LIB) && cd $(LIBEV_INC) && make clean

cleanhx:
	rm -rf $(FISH) $(FISH_INC)/*o
	rm -rf $(LIBS_DIR)/$(HX_LIB) && cd $(HX_INC) && make clean

clean:
	rm -rf $(FISH) $(FISH_INC)/*o
	
