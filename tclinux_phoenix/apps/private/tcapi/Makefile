#
#Configured directories
#
#prefix	=	/usr/local
#exec_prefix	=	/usr/local
#libdir	=	$(exec_prefix)/lib/
#libdir= ../lib
libdir= $(LIB_INSTALL_DIR)

#CROSS=mips-linux-
#CC	=$(CROSS)gcc
DSO	=	$(CC)
DSOFLAGS    =   -Wall  $(LIBINSTALL_LDFLAGS) $(TCAPI_LIB) $(COMPILEOPTION_LIB)
LN	=	/bin/ln -s
RM	=	/bin/rm -f
#OPTIM=	-fPIC -Os -g
STRIP=	$(CROSS)strip
#TCAPILIB= -lm -L $(libdir) -ltcapi

#LD=	$(CROSS)ld 
ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os -Wall
else
CFLAGS += -O2 -Wall
endif

ifneq ($(TC_CFLAGS), )
        CFLAGS+=$(TC_CFLAGS)
endif

CFLAGS+= -I$(LIB_INSTALL_DIR)

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
		
OBJS	=	tcapi.o
TARGETS	=	tcapi

all:  $(TARGETS)

tcapi_lib:
	$(MAKE) -C ../tcapi_lib clean
	$(MAKE) -C ../tcapi_lib
	$(MAKE) -C ../tcapi_lib  install
	
tcapi:	$(OBJS)
	$(DSO) $(DSOFLAGS) -o $(TARGETS) $(OBJS) 
	$(STRIP) $(TARGETS)
	
clean:
	$(RM) $(OBJS) $(TARGETS)
