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
CC_WARNINGS   = $(CC_WARNINGS_ALL) -Werror

#
# TARGETS and RULES
#
TARGET = $(OBJDIR)/siw.o

OBJECT_LIST = siw_events.o   \
              siw_trace.o    \
              siw_init.o     \
              siw_tmrfunc.o  \
              siw_itmr.o     \
              siw_log.o      \
              siw_mem.o      \
              siw_task.o     \
              siw_mq.o       \
              siw_sem.o      \
              siw_time.o     \
              siw_cards.o    

EXTRA_OBJECT = 

#		-I$(COMMON_INC) \

EXTRA_INCLUDE = -I$(SOURCE)     \
		-I$(PLFM_INC)   \
		-I$(FLASH_DIR)	\
		-I$(FRAMER_DS26502_DIR) \
		-I$(WIND_BASE)\target\h\tffs

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


#
# machine dependent targets
$(TARGET): $(OBJDIR) $(SOURCE)/depend.$(BSP_NAME) $(DEP_NAMES)
	$(LD) $(LDFLAGS) $(LD_PARTIAL_FLAGS) -o $@ $(DEP_NAMES)

# INCLUDE STANDARD RULES
#
include $(RULES_PATH)/Makefile.crules
include $(RULES_PATH)/rules.make

