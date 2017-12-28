#
# INCLUDE STANDARD DEFINITIONS
#

include $(ENV)/build_env

ifndef $(SOURCE)
SOURCE = $(CUR_DIR)
endif

ifndef $(OBJDIR)
OBJDIR = ./Obj
endif

ifndef $(PROJ_DIR)
PROJ_DIR = $(TOP_DIR)
endif

SUBDIR_NAME = $(BSP_NAME)
CUR_DIR     = $(shell pwd)

include $(RULES_PATH)/$(DEFINE_FILE)
CC_WARNINGS   = $(CC_WARNINGS_ALL) #-Werror

#
# TARGETS and RULES
#

TARGET = $(OBJDIR)/boot.o

ifdef TSS
OBJECT_LIST = sysALib.o      \
	          sysLib.o		 \
			  sysAUtils.o	 \
			  sysUtils.o	 \
			  motFcc2End.o	 \
			  m82xxDpramLib.o
else
OBJECT_LIST = sysALib.o      \
	          sysLib.o		 \
			  sysAUtils.o	 \
			  sysUtils.o	 \
			  motFcc2End.o	 \
			  m82xxDpramLib.o
endif
 
EXTRA_OBJECT = 

EXTRA_INCLUDE = -I$(SOURCE)     \
		-I$(PLFM_INC)   \
		-I$(WIND_BASE)\target\h\tffs

OBJECT_LIST += $(EXTRA_OBJECT)

DEP_NAMES = $(OBJECT_LIST:%.o=$(OBJDIR)/%.o)

ifdef TSS
C_OBJS = sysLib.o sysUtils.o motFcc2End.o m82xxDpramLib.o
else
C_OBJS = sysLib.o sysUtils.o motFcc2End.o m82xxDpramLib.o
endif

A_OBJS = sysALib.o sysAUtils.o

DEP_C_NAMES = $(C_OBJS:%.o=$(SOURCE)/%.c)
DEP_A_NAMES = $(A_OBJS:%.o=$(SOURCE)/%.s)

CC_OPTIM_NORMAL = -O0 -fstrength-reduce -fcombine-regs
CC_OPTIM_TARGET = -O0 -fvolatile

LIB_EXTRA        =  motFecEnd.a

default: $(TARGET)

# Add local targets/rules here
#
##############################################################

$(SOURCE)/depend.$(SUBDIR_NAME) : $(DEP_C_NAMES) $(DEP_A_NAMES)
	$(CC) -M $(CFLAGS) $(DEP_C_NAMES) > $@
	$(CC) -M $(CFLAGS_AS) $(DEP_A_NAMES) >> $@

	$(CP) $(BOOT_ALL)/dataSegPad.c $(BOOT_DIR)/dataSegPad.c
	$(CC) -c $(CFLAGS) $(CC_INCLUDE) $(BOOT_DIR)/dataSegPad.c -o $(OBJDIR)/dataSegPad.o
	- @ $(RM) $(BOOT_DIR)/dataSegPad.c

	$(CP) $(BOOT_ALL)/usrConfig.c $(BOOT_DIR)/usrConfig_st.c
	$(CC) -c $(CFLAGS) $(CC_INCLUDE) -DSTANDALONE $(BOOT_DIR)/usrConfig_st.c -o $(OBJDIR)/usrConfig_st.o
	- @ $(RM) $(BOOT_DIR)/usrConfig_st.c
	
#	$ @ $(RM) $@.tmp
#	$ @ $(AWK) /auto/mssbu-nproject/Unix_Project/tools/getname.awk $@ > $@.tmp
#	$ @ $(RM) $@
#	$ @ $(MV) $@.tmp $@
#	$ @ chmod a+w $@

#
# machine dependent targets

ifdef TSS
$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(BSP_NAME) $(DEP_NAMES)
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES) $(OBJDIR)/dataSegPad.o $(OBJDIR)/usrConfig_st.o
else	
$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(BSP_NAME) $(DEP_NAMES)
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES) $(OBJDIR)/dataSegPad.o $(OBJDIR)/usrConfig_st.o
endif

# INCLUDE STANDARD RULES
#
include $(RULES_PATH)/rules.make

mkmotFecEnd: motFecEnd.o
	$(AR) crus motFecEnd.a motFecEnd.o

.neverbuilt:
