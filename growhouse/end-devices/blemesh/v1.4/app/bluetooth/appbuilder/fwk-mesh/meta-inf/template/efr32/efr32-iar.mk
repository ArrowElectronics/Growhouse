####################################################################
# Makefile                                                         #
####################################################################

.SUFFIXES: # ignore builtin rules
.PHONY: all debug release clean .FORCE


####################################################################
# Definitions                                                      #
####################################################################

# uniq is a function which removes duplicate elements from a list
uniq = $(strip $(if $1,$(firstword $1) \
       $(call uniq,$(filter-out $(firstword $1),$1))))

DEVICE = $--partNumber--$
PROJECTNAME = $--deviceName--$

OBJ_DIR = build
EXE_DIR = exe
LST_DIR = lst


####################################################################
# Definitions of toolchain.                                        #
# You might need to do changes to match your system setup          #
####################################################################

#Set IARPATH environmental variable on your platform if you want to overwrite it.
IARPATH ?= "C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.0/arm/bin/"

WINE_PREFIX := wine
FORMAT_IAR_DEP_FILE = sed -i.bak s'/C:\\/\~\/\.wine\/drive_c\//'g $(@:%.o=%.d) ; \
                      sed -i.bak s'/[^C]:\\/\//'g $(@:%.o=%.d) ; \
                      sed -i.bak s'/\\\([^ ]\)/\/\1/'g $(@:%.o=%.d)

RMDIRS     := rm -rf
RMFILES    := rm -rf
ALLFILES   := /*.*
NULLDEVICE := /dev/null
SHELLNAMES := $(ComSpec)$(COMSPEC)

# Try autodetecting the environment
ifneq ($(SHELLNAMES),)
  QUOTE :="
  WINE_PREFIX :=
  FORMAT_IAR_DEP_FILE :=
  ifneq ($(COMSPEC),)
    ifeq ($(findstring cygdrive,$(shell set)),)
      # We were not on a cygwin platform
      NULLDEVICE := NUL
    endif
  else
    # Assume we are making on a Windows platform
    # This is a convenient place to override TOOLDIR, DO NOT add trailing
    # whitespace chars, they do matter !
    SHELL      := $(SHELLNAMES)
    RMDIRS     := rd /s /q
    RMFILES    := del /s /q
    ALLFILES   := \*.*
    NULLDEVICE := NUL
  endif
endif

# Create directories and do a clean which is compatible with parallell make
$(shell mkdir $(OBJ_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(EXE_DIR)>$(NULLDEVICE) 2>&1)
$(shell mkdir $(LST_DIR)>$(NULLDEVICE) 2>&1)
ifeq (clean,$(findstring clean, $(MAKECMDGOALS)))
  ifneq ($(filter $(MAKECMDGOALS),all debug release),)
    $(shell $(RMFILES) $(OBJ_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
    $(shell $(RMFILES) $(EXE_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
    $(shell $(RMFILES) $(LST_DIR)$(ALLFILES)>$(NULLDEVICE) 2>&1)
  endif
endif

AS = $(WINE_PREFIX) $(QUOTE)$(IARPATH)/iasmarm.exe$(QUOTE)
CC = $(WINE_PREFIX) $(QUOTE)$(IARPATH)/iccarm.exe$(QUOTE)
LD = $(WINE_PREFIX) $(QUOTE)$(IARPATH)/ilinkarm.exe$(QUOTE)
AR = $(WINE_PREFIX) $(QUOTE)$(IARPATH)/iarchive.exe$(QUOTE)
EL = $(WINE_PREFIX) $(QUOTE)$(IARPATH)/ielftool.exe$(QUOTE)
BG = $--bgbuildExe--$


####################################################################
# Flags                                                            #
####################################################################

override ASMFLAGS += \
-S \
-s+ \
-M"<>" \
-w+ \
-r \
--cpu Cortex-M4F \
--fpu VFPv4_sp \
-DCONFIGURATION_HEADER=\"$--generatedHeader--$\" \
-D__NO_SYSTEM_INIT \
-D$(DEVICE)

override CFLAGS += \
--enum_is_int \
--silent \
--no_cse \
--no_unroll \
--no_inline \
--no_code_motion \
--no_tbaa \
--no_clustering \
--no_scheduling \
--debug \
-e \
-Om \
--use_c++_inline \
--cpu=Cortex-M4F \
--fpu=VFPv4_sp \
--endian=little \
--diag_suppress=Pa050 \
--dependencies=m $(OBJ_DIR)/$*.d \
--dlib_config ../INC/c/DLib_Config_Normal.h \
-DCONFIGURATION_HEADER=\"$--generatedHeader--$\" \
-DBOOKKEEPING_HEADER=\"ble-soc-bookkeeping.h\" \
-DCALLBACKS_HEADER=\"ble-soc-callbacks.h\" \
-D__NO_SYSTEM_INIT \
-D$(DEVICE)

override LDFLAGS += \
--silent \
--config ./bleDemo.icf \
--semihosting \
--entry __iar_program_start \
--keep __iar_program_start \
--no_range_reservations \
--vfe

INCLUDEPATHS += \
  -I. \
  -I./inc \
$--includePathsMak--$


####################################################################
# Files                                                            #
####################################################################

GATTDB_PROJ = \
$--bgproj-assembly-generate--$

GATTDB_SRC = \
gatt_db.c

GATTDB_HDR = \
gatt_db.h

GATTDB_CONST = \
constants

C_SRC += \
$(GATTDB_SRC) \
$--includeFilesMak--$

s_SRC += 

S_SRC += 

LIBS = \
$--includeLibFilesMak--$


####################################################################
# Rules                                                            #
####################################################################

C_FILES = $(notdir $(C_SRC) )
S_FILES = $(notdir $(S_SRC) $(s_SRC) )
#make list of source paths, uniq removes duplicate paths
C_PATHS = $(call uniq, $(dir $(C_SRC) ) )
S_PATHS = $(call uniq, $(dir $(S_SRC) $(s_SRC) ) )

C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.o))
S_OBJS = $(if $(S_SRC), $(addprefix $(OBJ_DIR)/, $(S_FILES:.S=.o)))
s_OBJS = $(if $(s_SRC), $(addprefix $(OBJ_DIR)/, $(S_FILES:.s=.o)))
C_DEPS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.d))
OBJS = $(C_OBJS) $(S_OBJS) $(s_OBJS)

vpath %.c $(C_PATHS)
vpath %.s $(S_PATHS)
vpath %.S $(S_PATHS)

# Default build is debug build
all:      debug

#debug:    CFLAGS += -O0 -g3
debug:    $(EXE_DIR)/$(PROJECTNAME).hex

#release:  CFLAGS += -O0 -g3
release:  $(EXE_DIR)/$(PROJECTNAME).hex


# Generate gatt_db files from bgproj file
$(GATTDB_SRC): $(GATTDB_PROJ) .FORCE
	@echo "Generating file: $@"
	@$(BG) -ng $(GATTDB_PROJ)

# Create objects from C SRC files
$(OBJ_DIR)/%.o: %.c
	@echo "Building file: $<"
	@$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<
	@$(FORMAT_IAR_DEP_FILE)

# Assemble .s/.S filesdocs
$(OBJ_DIR)/%.o: %.s
	@echo "Assembling $<"
	@$(AS) $(ASMFLAGS) $(INCLUDEPATHS) -c-o $@ $<

$(OBJ_DIR)/%.o: %.S
	@echo "Assembling $<"
	@$(AS) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

# Link
$(EXE_DIR)/$(PROJECTNAME).out: $(OBJS) $(LIBS)
	@echo "Linking target: $@"
	@$(LD) $(LDFLAGS) $^ -o $@

# Create Intel hex file
$(EXE_DIR)/$(PROJECTNAME).hex: $(EXE_DIR)/$(PROJECTNAME).out
	@echo "Creating Intel hex file: $@"
	@$(EL) --ihex --silent $< $@

clean:
ifeq ($(filter $(MAKECMDGOALS),all debug release),)
	$(RMDIRS) $(OBJ_DIR) $(LST_DIR) $(EXE_DIR)
endif

# include auto-generated dependency files (explicit rules)
ifneq (clean,$(findstring clean, $(MAKECMDGOALS)))
-include $(C_DEPS)
endif


