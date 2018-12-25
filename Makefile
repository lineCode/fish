LUA_STATICLIB := ./Bin/Debug/liblua.a
LUA_LIB ?= liblua.a
LUA_INC ?= ./3rd/lua/src

OOLUA_STATICLIB := ./Bin/Debug/liboolua.a
OOLUA_LIB ?= liboolua.a
OOLUA_INC ?= ./3rd/oolua/include
OOLUA_PATH ?= ./3rd/oolua
OOLUA_LIB_PATH ?= ./3rd/oolua/bin/Debug

LIBEV_STATICLIB := ./Bin/Debug/libev.a
LIBEV_LIB ?= libev.a
LIBEV_INC ?= ./3rd/libev

HX_STATICLIB := ./Bin/Debug/libhx.a
HX_LIB ?= libhx.a
HX_INC ?= ./hx

FISH := ./Bin/Debug/fish
FISH_INC ?= ./server


LIBS_DIR := ./Bin/Debug


all: $(LUA_STATICLIB) $(OOLUA_STATICLIB) $(LIBEV_STATICLIB) $(HX_STATICLIB) $(FISH)

$(LUA_STATICLIB) :
	cd $(LUA_INC)&& $(MAKE) linux
	mv $(LUA_INC)/$(LUA_LIB) $(LIBS_DIR) 
	mv $(LUA_INC)/liblua.so $(LIBS_DIR) 

$(OOLUA_STATICLIB) :
	cd $(OOLUA_PATH) && $(MAKE)
	mv $(OOLUA_LIB_PATH)/liboolua_d.a $(LIBS_DIR)/$(OOLUA_LIB)

$(LIBEV_STATICLIB) :
	cd $(LIBEV_INC) && $(MAKE)
	mv $(LIBEV_INC)/.libs/libev.a $(LIBS_DIR)

$(HX_STATICLIB) :
	cd $(HX_INC) && $(MAKE)
	mv $(HX_INC)/$(HX_LIB) $(LIBS_DIR)
 
$(FISH) :
	cd $(FISH_INC) && $(MAKE)
	mv $(FISH_INC)/fish $(LIBS_DIR) 
 

clean :
	rm -rf $(LIBS_DIR)/fish  $(FISH_INC)/*o
	cd $(HX_INC) && make clean
	rm -rf $(LIBS_DIR)/liblua.a $(LIBS_DIR)/liblua.so $(LUA_INC)/*.o
	rm -rf $(LIBS_DIR)/liboolua.a && cd $(OOLUA_PATH) && make clean
	rm -rf $(LIBS_DIR)/libev.a && cd $(LIBEV_INC) && make clean

