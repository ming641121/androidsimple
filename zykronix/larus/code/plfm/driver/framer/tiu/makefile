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

CC_WARNINGS   = 
# CC_WARNINGS   = $(CC_WARNINGS_ALL) -Werror 

#
# TARGETS and RULES
#
TARGET = $(OBJDIR)/tiu.o

OBJECT_LIST =   tiu_ds26502.o tiu_event.o


EXTRA_OBJECT = 


EXTRA_INCLUDE = -I$(SOURCE)     \
		-I$(PLFM_INC)   \
		-I$(FLASH_DIR)	\
		-I$(WIND_BASE)\target\h   \
		-I$(WIND_BASE)\target\h\tffs  \
		-I$(FRAMER_DS26502_DIR) \
		-I$(SNMP_LARUS_INC) \
		-I$(SNMP_LUMA_DIR) \
		-I$(SNMP_LUMA_INC) \
		-I$(SNMP_SNMPD_INC) \
		-I$(SNMP_SNMPD_UNIX) \
		-I$(SNMP_LIB_DIR)

OBJECT_LIST += $(EXTRA_OBJECT)

DEP_NAMES = $(OBJECT_LIST:%.o=$(OBJDIR)/%.o)

C_OBJS = $(OBJECT_LIST)
A_OBJS = 

DEP_C_NAMES = $(C_OBJS:%.o=$(SOURCE)/%.c)
DEP_A_NAMES = $(A_OBJS:%.o=$(SOURCE)/%.s)

CC_OPTIM_NORMAL = -O0 -fstrength-reduce -fcombine-regs
CC_OPTIM_TARGET = -O0 -fvolatile

LIB_EXTRA        =  motFecEnd.a

default: $(TARGET)


# Add local targets/rules here
#
##############################################################

ADDED_CFLAGS =	-DVXWORKS  -DE_CLOCK -TIU_DEBUG

#
# machine dependent targets
$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(BSP_NAME) $(DEP_NAMES) 
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES)

# INCLUDE STANDARD RULES
#
include $(RULES_PATH)/Makefile.crules
include $(RULES_PATH)/rules.make

