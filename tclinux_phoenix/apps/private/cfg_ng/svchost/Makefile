


CFLAGS += -fPIC -O2 -Werror  -I../install/include

LIB_LIBS = -L../install/lib -lubus -lubox  -lpthread -ldl

EXE_LIBS = -L./  -lhostservice  -L../install/lib -lubus -lubox  -lpthread -ldl

EXE_OBJS = svchost_parse.o  svchost.o

LIB_OBJS = svchost_api.o   svchost_loop.o

SVCHOST_EXE = svchost 

SVCHOST_LIB = libhostservice.so

TARGETS	=   $(SVCHOST_LIB)  $(SVCHOST_EXE)


.SUFFIXES:  .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<


all:  $(TARGETS)

$(SVCHOST_LIB): $(LIB_OBJS)
	$(CC)  -shared   -o  $@  $^  $(LIB_LIBS)
	
$(SVCHOST_EXE): $(EXE_OBJS)
	$(CC)   -o  $@  $^   $(EXE_LIBS) 
	

install:
	cp $(SVCHOST_LIB)   ../install/lib
	cp $(SVCHOST_EXE)  ../install/bin
	cp svchost_api.h  ../install/include
	cp svchost_id.h  ../install/include
	cp svchost_evt.h  ../install/include
	
clean:
	-rm  *.o  $(SVCHOST_EXE)  $(SVCHOST_LIB)