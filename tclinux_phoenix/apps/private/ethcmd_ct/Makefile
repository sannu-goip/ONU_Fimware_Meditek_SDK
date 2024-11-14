#CROSS=mips-linux-

#CC=$(CROSS)gcc
AR=$(CROSS)ar
LD=$(CROSS)ld
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size
LIBDIR=$(CROSS_PATH)/lib


#CFLAGS= -mips1 -msoft-float -DETHCMD_DBG -DETHCMD_EXTENSION
#CFLAGS= -mips1 -msoft-float 
ifneq ($(TC_CFLAGS), )
        CFLAGS+=$(TC_CFLAGS)
endif

OBJS= ethcmd.o libcmd.o ethercmd.o libmii.o ether.o
EXECS= ethcmd

all: ethcmd

.c.o:
	$(CC) -c $(CFLAGS) $<

ethcmd: $(OBJS) 
	$(CC) $(OBJS) -o $@
	$(STRIP) --strip-unneeded $(EXECS)

clean:
	rm -f *.o *.a $(EXECS)
