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

RULES_PATH = $(PROJ_DIR)/make_rules
include $(RULES_PATH)/tss.defs
CC_WARNINGS   = $(CC_WARNINGS_ALL) -Werror

SUBDIR_NAME = $(BSP_NAME)
CUR_DIR     = $(shell pwd)

TGT_DIR     = $(WIND_BASE)/target
#
# TARGETS and RULES
#

TARGET = $(OBJDIR)/bootload.o


OBJECT_LIST = romInit.o	sysAUtils.o


EXTRA_OBJECT = 

EXTRA_INCLUDE = -I$(SOURCE)     \
		-I$(PLFM_INC)

OBJECT_LIST += $(EXTRA_OBJECT)

DEP_NAMES = $(OBJECT_LIST:%.o=$(OBJDIR)/%.o)

C_OBJS = 

A_OBJS = romInit.o sysAUtils.o

DEP_C_NAMES = $(C_OBJS:%.o=$(SOURCE)/%.c)
DEP_A_NAMES = $(A_OBJS:%.o=$(SOURCE)/%.s)

BOOT_FLAGS = -DRAM_WIDTH_64 -DENABLE_BOOT_TRACE=1 -DDEBUG_CONFIG_ALL=1 -DFW_BOOT

ADDED_CFLAGS    = -g $(BOOT_FLAGS)

default: $(TARGET)

# Add local targets/rules here
#
##############################################################

$(SOURCE)/depend.$(SUBDIR_NAME).boot : $(DEP_C_NAMES) $(DEP_A_NAMES) $(SOURCE)/makefile $(ENV)/build_env
	$(CC) -M $(CFLAGS_AS) $(CC_INCLUDE) $(DEP_A_NAMES) > $@
	
	$(CP) $(BOOT_ALL)/bootInit.c $(BOOT_DIR)/bootInit_st.c
	$(CC) -c $(CFLAGS) $(CC_INCLUDE) -DRAM_DST_ADRS=0x$(RAM_LOW_ADRS) $(BOOT_DIR)/bootInit_st.c -o $(OBJDIR)/bootInit_st.o
	- @ $(RM) $(BOOT_DIR)/bootInit_st.c
	
#	$ @ $(RM) $@.tmp
#	$ @ $(AWK) /auto/mssbu-nproject/Unix_Project/tools/getname.awk $@ > $@.tmp
#	$ @ $(RM) $@
#	$ @ $(MV) $@.tmp $@
#	$ @ chmod a+w $@


#
# machine dependent targets
$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(SUBDIR_NAME).boot $(DEP_NAMES)
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES) $(OBJDIR)/bootInit_st.o

# INCLUDE STANDARD RULES
#
include $(RULES_PATH)/rules.nodepmake
include $(SOURCE)/depend.$(SUBDIR_NAME).boot

.neverbuilt:
