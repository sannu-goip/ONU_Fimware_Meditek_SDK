include ../../apps-function.mk

INCLUDE		= -I$(LIB_INSTALL_DIR)
INCLUDE += -I$(APP_TCAPILIB_DIR)
OBJS		  =	map_hal.o
HEADER		=	map_hal.h
LIBS	    = -L $(LIB_INSTALL_DIR) -ltcapi
LIB_NAME	=	libmap_hal.so

RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+= -fPIC $(INCLUDE) $(TC_CFLAGS)

all: $(LIB_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(LIB_NAME): clean $(OBJS)
	$(CC) -shared -o $(LIB_NAME) $(OBJS) $(LIBS)
	$(STRIP) --strip-unneeded $(LIB_NAME)

install:
	install $(LIB_NAME) $(LIB_INSTALL_DIR)
	cp $(HEADER) $(LIB_INSTALL_DIR)
	cp  $(LIB_INSTALL_DIR)/$(LIB_NAME) $(FILESYSTEM_DIR)/lib/
	$(call EcntAPPReleaseLibInstall, $(LIB_INSTALL_DIR), $(LIB_NAME))
	
clean:
	$(RM) $(OBJS) $(LIB_NAME)
	$(RM) $(LIB_INSTALL_DIR)/$(LIB_NAME)
	$(RM) $(LIB_INSTALL_DIR)/$(HEADER)
	$(RM) $(FILESYSTEM_DIR)/lib/$(LIB_NAME)

