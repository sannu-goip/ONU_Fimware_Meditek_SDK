#
#Configured directories
#
#prefix	=	/usr/local
#exec_prefix	=	/usr/local
#libdir	=	$(exec_prefix)/lib
libdir=	$(LIB_INSTALL_DIR)
publiclibdir= $(PUBLIC_LIB_DIR)
#
#Install commands
#
INSTALL=	/usr/bin/install -c
INSTALL_DIR	=	$(INSTALL) -d
INSTALL_LIB	=	$(LIBTOOL)	$(INSTALL) -m 755

#CROSS=mips-linux-gnu-

#CC=$(CROSS)gcc -mips32r2 -msoft-float -muclibc
AR=$(CROSS)ar
LD=$(CROSS)ld 
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size
LIBDIR=$(CROSS_PATH)/lib

DSOFLAGS=	-Wl,-soname,libtcapi.so.1,-rpath,$(libdir) -shared $(OPTIM)
LN      =	/bin/ln -fs
RM      =	/bin/rm -f
OPTIM	=	-fPIC -Os -g



#CFLAGS= -mips1 -msoft-float 
CFLAGS +=-fPIC -Os -g  -Wall

ifneq ($(TC_CFLAGS), )
        CFLAGS+=$(TC_CFLAGS)
endif

ifeq ($(WITHVOIP),1)
CFLAGS += -DWITHVOIP
endif #WITHVOIP

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

LIBOBJS =       tcapi.o
ifneq ($(strip $(TCSUPPORT_CDS_PASSWD_ENCRYPT)),)
LIBOBJS += crypt_func.o
endif
LIBTCAPI=       libtcapi.so.1.4
TARGETS =       $(LIBTCAPI)
SOFTLINK=		libtcapi.so libtcapi.so.1

CFLAGS += $(LIBINSTALL_CFLAGS)
ifneq ($(TCSUPPORT_CFG_NG),)
CFLAGS += -DCFG2_FLAG  -I$(APP_CFG_NG_DIR)/service/cfg
LIBCFG2 = $(LIBINSTALL_LDFLAGS) -lcfg_clisvc
endif
all:  $(TARGETS)

$(LIBOBJS): tcapi.h
tcapi.o:tcapi.h

libtcapi.so.1.4: $(LIBOBJS)
	$(CC) $(CFLAGS) $(DSOFLAGS) -o libtcapi.so.1.4 $(LIBOBJS) $(LIBCFG2)
	$(STRIP) --strip-unneeded $(LIBTCAPI)
	$(RM) libtcapi.so libtcapi.so.1
	$(LN) libtcapi.so.1.4 libtcapi.so
	$(LN) libtcapi.so.1.4 libtcapi.so.1

install:
	$(INSTALL_DIR) $(BUILDROOT)$(libdir) $(LIB_INSTALL_DIR)
#	$(RM) $(BUILDROOT)$(libdir)/libtcapi.so*
	$(RM) $(LIB_INSTALL_DIR)/libtcapi.so*
#	cp -rf libtcapi.so* $(BUILDROOT)$(publiclibdir)
#	cp -rf libtcapi.so* $(LIB_DIR)
	cp -rf libtcapi.so* $(LIB_INSTALL_DIR)

#	cp -rf libtcapi.h $(BUILDROOT)$(publiclibdir)
#	cp -rf libtcapi.h $(LIB_DIR)	
	cp -rf libtcapi.h $(LIB_INSTALL_DIR)

clean:
	$(RM) $(OBJS) $(TARGETS) $(LIBOBJS) $(SOFTLINK)
