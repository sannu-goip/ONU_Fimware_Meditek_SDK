#
#Configured directories
#
#prefix	=	/usr/local
#exec_prefix	=	/usr/local
#libdir	=	$(exec_prefix)/lib
#
#Install commands
#
INSTALL=	/usr/bin/install -c
INSTALL_DIR	=	$(INSTALL) -d
INSTALL_LIB	=	$(LIBTOOL)	$(INSTALL) -m 755

CFLAGS += -fPIC -O2 -Werror -I$(LIB_INSTALL_DIR)

ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif

LDFLAGS = -L$(LIB_INSTALL_DIR) -lcares
ifneq ($(strip $(TCSUPPORT_CHARSET_CHANGE)),)	
LDFLAGS += -lcharset -liconv
endif
UTILITY_LIB_LIBS = 

UTILITY_LIB_OBJS = ecnt_utility.o
UTILITY_LIB = libecntutility.so
HEADER = ecnt_utility.h

TARGETS = $(UTILITY_LIB)

all: $(TARGETS)

%.o:%.c
	$(CC)   $(CFLAGS)  $(TC_CFLAGS)  -c $< $(LDFLAGS)


$(UTILITY_LIB): $(UTILITY_LIB_OBJS)
	$(CC)   -shared   -o  $@   $^   $(UTILITY_LIB_LIBS) $(LDFLAGS)
	
clean:
	-rm  $(UTILITY_LIB_OBJS) $(TARGETS)
	-rm  $(LIB_INSTALL_DIR)/$(TARGETS)
	-rm  $(LIB_INSTALL_DIR)/$(HEADER)

install:
	$(INSTALL_DIR) $(LIB_INSTALL_DIR)
	$(INSTALL_LIB) $(TARGETS) $(LIB_INSTALL_DIR)
	cp $(HEADER) $(LIB_INSTALL_DIR)
