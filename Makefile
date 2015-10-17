LUA_STATICLIB := ./libs/liblua.a
LUA_LIB ?= liblua.a
LUA_INC ?= ./lib/3rd/lua/src

PBC_STATICLIB := ./libs/libpbc.a
PBC_LIB ?= libpbc.a
PBC_INC ?= ./lib/3rd/pbc

CORE_STATICLIB := ./libs/libcore.a
CORE_LIB ?= libcore.a
CORE_INC ?= ./lib/core

LPEG_SO := ./libs/lpeg.so
LPEG_NAME ?= lpeg.so
LPEG_INC ?= ./lib/3rd/lpeg

CJSON_SO := ./libs/cjson.so
CJSON_NAME ?= cjson.so
CJSON_INC ?= ./lib/3rd/cjson

LFS_SO := ./libs/lfs.so
LFS_NAME ?= lfs.so
LFS_INC ?= ./lib/3rd/luafilesystem

PROTOBUF_SO := ./libs/protobuf.so
PROTOBUF_NAME ?= protobuf.so
PROTOBUF_INC ?= ./lib/3rd/pbc/binding/lua

BSON_SO := ./libs/bson.so
BSON_NAME ?= bson.so
BSON_INC ?= ./lib/3rd/bson

SERIALIZE_SO := ./libs/serialize.so
SERIALIZE_NAME ?= serialize.so
SERIALIZE_INC ?= ./lib/3rd/serialize

FISH := ./libs/fish
FISH_INC ?= ./fish


LIBS_DIR := ./libs


all: $(LUA_STATICLIB) $(PBC_STATICLIB) $(LPEG_SO) $(CJSON_SO) $(LFS_SO) $(PROTOBUF_SO) $(BSON_SO) $(SERIALIZE_SO) $(CORE_STATICLIB) $(FISH)

$(LUA_STATICLIB) :
	cd $(LUA_INC)&& $(MAKE) linux
	mv $(LUA_INC)/$(LUA_LIB) $(LIBS_DIR) 
	mv $(LUA_INC)/liblua.so $(LIBS_DIR) 

$(PBC_STATICLIB) :
	cd $(PBC_INC)&& $(MAKE)
	mv $(PBC_INC)/build/$(PBC_LIB) $(LIBS_DIR) 

$(CORE_STATICLIB) :
	cd $(CORE_INC) && $(MAKE)
	mv $(CORE_INC)/$(CORE_LIB) $(LIBS_DIR)

$(LPEG_SO) :
	cd $(LPEG_INC)&& $(MAKE)
	mv $(LPEG_INC)/$(LPEG_NAME) $(LIBS_DIR) 

$(CJSON_SO) :
	cd $(CJSON_INC)&& $(MAKE)
	mv $(CJSON_INC)/$(CJSON_NAME) $(LIBS_DIR) 

$(LFS_SO) :
	cd $(LFS_INC)&& $(MAKE)
	mv $(LFS_INC)/src/$(LFS_NAME) $(LIBS_DIR) 

$(PROTOBUF_SO) :
	cd $(PROTOBUF_INC)&& $(MAKE)
	mv $(PROTOBUF_INC)/$(PROTOBUF_NAME) $(LIBS_DIR) 

$(BSON_SO) :
	cd $(BSON_INC) && $(MAKE)
	mv $(BSON_INC)/$(BSON_NAME) $(LIBS_DIR) 

$(SERIALIZE_SO) :
	cd $(SERIALIZE_INC) && $(MAKE)
	mv $(SERIALIZE_INC)/$(SERIALIZE_NAME) $(LIBS_DIR) 
 
$(FISH) :
	cd $(FISH_INC) && $(MAKE)
	mv $(FISH_INC)/fish $(LIBS_DIR) 
 

clean :
	rm -rf $(LIBS_DIR)/fish  $(FISH_INC)/*o
	rm -rf $(LIBS_DIR)/libcore.a $(CORE_INC)/*.o
	rm -rf $(LIBS_DIR)/liblua.a $(LIBS_DIR)/liblua.so $(LUA_INC)/*.o
	rm -rf $(LIBS_DIR)/libpbc.a $(PBC_INC)/build/o
	rm -rf $(LIBS_DIR)/lpeg.so $(LPEG_INC)/*.o
	rm -rf $(LIBS_DIR)/cjson.so $(CJSON_INC)/*.o
	rm -rf $(LIBS_DIR)/lfs.so $(LFS_INC)/*.o
	rm -rf $(LIBS_DIR)/bson.so $(BSON_INC)/*.o
	rm -rf $(LIBS_DIR)/serialize.so $(SERIALIZE_INC)/*.o
	
