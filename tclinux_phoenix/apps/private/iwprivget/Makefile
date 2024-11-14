##################################
#####Configured directories#######
##################################
INCLUDE			= -I$(KERNEL_HEADER) -I$(APP_WRIELESSTOOL_DIR)
		
USRBINDIR		= $(FILESYSTEM_DIR)/userfs/bin

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
IWPRIVGET_CFLAGS	= -O2 -Wall $(INCLUDE) $(TC_CFLAGS) 

##################################
########Configured output#########
##################################
OBJS	=	iwprivget.o 
TARGETS	=	iwprivget

all: $(TARGETS)

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(IWPRIVGET_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(IWPRIVGET_CFLAGS) -o $(TARGETS) $(OBJS)
	$(STRIP) --strip-unneeded $(TARGETS)
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(RM) $(USRBINDIR)/$(TARGETS)
