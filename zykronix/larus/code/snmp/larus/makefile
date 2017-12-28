#
# MAKEFILE
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

TARGET = $(OBJDIR)/snmpKV.o

OBJECT_LIST   =  k_LarusSts.o	\
				 v_LarusSts.o				 

EXTRA_OBJECT  =

EXTRA_INCLUDE = -I$(BOOT_DIR)   \
				-I$(PLFM_INC)	\
				-I$(SNMP_LARUS_INC)	\
				-I$(SNMP_LUMA_DIR) \
				-I$(SNMP_LUMA_INC) \
				-I$(SNMP_SNMPD_INC) \
				-I$(SNMP_SNMPD_UNIX) \
				-I$(SNMP_LIB_DIR)

OBJECT_LIST += $(EXTRA_OBJECT)

DEP_NAMES = $(OBJECT_LIST:%.o=$(OBJDIR)/%.o)

CPP_OBJS = 
C_OBJS =         k_LarusSts.o	\
				 v_LarusSts.o
A_OBJS =

DEP_CPP_NAMES = $(CPP_OBJS:%.o=$(SOURCE)/%.cpp)
DEP_C_NAMES   = $(C_OBJS:%.o=$(SOURCE)/%.c)
DEP_A_NAMES   = $(A_OBJS:%.o=$(SOURCE)/%.s)

#CC_OPTIM_NORMAL = -O0 -fstrength-reduce -fcombine-regs
#CC_OPTIM_TARGET = -O0 -fvolatile

default: $(TARGET)

# Add local targets/rules here
#
##############################################################

ADDED_CFLAGS =	-DVXWORKS

#
# machine dependent targets

$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(BSP_NAME) $(DEP_NAMES)
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES)

# INCLUDE STANDARD RULES
#

# Please be sure to pick the right Makefile for your development. For
# example, refer to chart below to determine which Makefile to use:
# - if you are writing both C and C++ code then use Makefile.rules.  
# - if you are writing only C code then use Makefile.crules
# - if you are writing only C++ code then use Makefile.c++rules
#
#include $(RULES_PATH)/Makefile.rules
include $(RULES_PATH)/Makefile.crules
#include $(RULES_PATH)/Makefile.c++rules
include $(RULES_PATH)/rules.make
