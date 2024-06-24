# Architecture directories
ARCHITECTURE_DIR = efr32
BUILD_DIR = build
OUTPUT_DIR = $(BUILD_DIR)/$(ARCHITECTURE_DIR)
LST_DIR = lst
PROJECTNAME = $--deviceName--$

# Stack and submodule directories
GLOBAL_BASE_DIR     = $--halDirFromProjFs--$/..

SOURCE_FILES = \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/dmadrv/src/dmadrv.c \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/gpiointerrupt/src/gpiointerrupt.c \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/rtcdrv/src/rtcdriver.c \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/sleep/src/sleep.c \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/tempdrv/src/tempdrv.c \
$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/ustimer/src/ustimer.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_adc.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_cmu.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_core.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_cryotimer.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_emu.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_gpio.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_i2c.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_ldma.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_leuart.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_mpu.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_msc.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_prs.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_rmu.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_rtcc.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_system.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_timer.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_usart.c \
$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/src/em_wdog.c \
$--halDirFromProjBs:\/--$/../../$--deviceDir:\/--$/SiliconLabs/$--chipFamily;U--$/Source/system_$--chipFamily;L--$.c \
./afv2-bookkeeping.c \
./call-command-handler.c \
./callback-stub.c \
./stack-handler-stub.c \
$--frameworkDirFromProjFs:\/--$/util/af-event.c \
$--frameworkDirFromProjFs:\/--$/util/af-main-common.c \
$--frameworkDirFromProjFs:\/--$/util/af-main-soc.c \
$--frameworkDirFromProjFs:\/--$/security/af-node.c \
$--frameworkDirFromProjFs:\/--$/security/af-security-common.c \
$--frameworkDirFromProjFs:\/--$/security/af-trust-center.c \
$--frameworkDirFromProjFs:\/--$/util/attribute-size.c \
$--frameworkDirFromProjFs:\/--$/util/attribute-storage.c \
$--frameworkDirFromProjFs:\/--$/util/attribute-table.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/assert-crash-handlers.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/button.c \
$--halDirFromProjFs:\/--$/plugin/buzzer/buzzer-efr32.c \
$--frameworkDirFromProjFs:\/--$/util/client-api.c \
$--frameworkDirFromProjFs:\/--$/../util/serial/command-interpreter2.c \
$--halDirFromProjFs:\/--$/micro/generic/crc.c \
$--frameworkDirFromProjFs:\/--$/security/crypto-state.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/cstartup-common.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/diagnostic.c \
$--frameworkDirFromProjFs:\/--$/../../stack/config/ember-configuration.c \
$--halDirFromProjFs:\/--$/micro/generic/endian.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/faults-v7m.s79 \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/isr-stubs.s79 \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/led.c \
$--frameworkDirFromProjFs:\/--$/../util/common/library.c \
$--halDirFromProjFs:\/--$/micro/generic/mem-util.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/mfg-token.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/micro-common.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/micro.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/mpu.c \
./cli.c \
$--frameworkDirFromProjFs:\/--$/cli/core-cli.c \
$--frameworkDirFromProjFs:\/--$/cli/network-cli.c \
$--frameworkDirFromProjFs:\/--$/cli/option-cli.c \
$--frameworkDirFromProjFs:\/--$/cli/plugin-cli.c \
$--frameworkDirFromProjFs:\/--$/util/print.c \
$--frameworkDirFromProjFs:\/--$/util/print-formatter.c \
$--frameworkDirFromProjFs:\/--$/util/process-cluster-message.c \
$--frameworkDirFromProjFs:\/--$/util/process-global-message.c \
$--halDirFromProjFs:\/--$/micro/generic/random.c \
$--frameworkDirFromProjFs:\/--$/../util/security/security-address-cache.c \
$--frameworkDirFromProjFs:\/--$/cli/security-cli.c \
$--halDirFromProjFs:\/--$/plugin/serial/serial.c \
$--halDirFromProjFs:\/--$/plugin/serial/cortexm/efm32/com.c \
$--frameworkDirFromProjFs:\/--$/util/service-discovery-common.c \
$--frameworkDirFromProjFs:\/--$/util/service-discovery-soc.c \
$--halDirFromProjFs:\/--$/ember-base-configuration.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/sleep-efm32.c \
$--halDirFromProjFs:\/--$/micro/generic/token-def.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/token.c \
$--halDirFromProjFs:\/--$/micro/cortexm3/efm32/ext-device.c \
$--frameworkDirFromProjFs:\/--$/util/message.c \
$--frameworkDirFromProjFs:\/--$/util/multi-network.c \
$--frameworkDirFromProjFs:\/--$/util/time-util.c \
$--frameworkDirFromProjFs:\/--$/util/util.c \
$--frameworkDirFromProjFs:\/--$/cli/zcl-cli.c \
$--frameworkDirFromProjFs:\/--$/cli/zdo-cli.c \
$--frameworkDirFromProjFs:\/--$/../util/zigbee-framework/zigbee-device-common.c \
$--frameworkDirFromProjFs:\/--$/../util/zigbee-framework/zigbee-device-library.c \
$--includeFilesMak--$ \
$--pluginFilesMake--$ \
$--callbackStubsMake--$ \

LIB_FILES = \
$--includeLibFiles--$ \
$--pluginLibraryFilesMake--$ \
$--pluginOptionFiles--$ \

CDEFS = -D$--bootloader--$ \
-DCORTEXM3 \
-DCORTEXM3_EFR32 \
-DCORTEXM3_EFR32_MICRO \
-DCORTEXM3_EFM32_MICRO \
-DEFR32_SERIES$--partHardware.series--$_CONFIG$--partHardware.device_configuration--$_MICRO \
-D$--chipFamily;U--$ \
-D$--partNumber--$ \
-DATTRIBUTE_STORAGE_CONFIGURATION=\"app/builder/$--deviceName--$/$--deviceName--$_endpoint_config.h\" \
-DCONFIGURATION_HEADER=\"app/framework/util/config.h\" \
-DGENERATED_TOKEN_HEADER=\"app/builder/$--deviceName--$/$--tokenHeader--$\" \
-DPLATFORM_HEADER=\"$--halDirFromStackFs:\/--$/micro/cortexm3/compiler/$--toolchainName--$.h\" \
-DZA_GENERATED_HEADER=\"app/builder/$--deviceName--$/$--deviceName--$.h\" \
$--dashDMacrosMak:"\"--$ \

ASMDEFS = -D$--bootloader--$ \
-DCORTEXM3 \
-DCORTEXM3_EFR32 \
-DCORTEXM3_EFR32_MICRO \
-DCORTEXM3_EFM32_MICRO \
-DEFR32_SERIES$--partHardware.series--$_CONFIG$--partHardware.device_configuration--$_MICRO \
-D$--chipFamily;U--$ \
-D$--partNumber--$ \
-DATTRIBUTE_STORAGE_CONFIGURATION=\"app/builder/$--deviceName--$/$--deviceName--$_endpoint_config.h\" \
-DCONFIGURATION_HEADER=\"app/framework/util/config.h\" \
-DGENERATED_TOKEN_HEADER=\"app/builder/$--deviceName--$/$--tokenHeader--$\" \
-DPLATFORM_HEADER=\"$--halDirFromStackFs:\/--$/micro/cortexm3/compiler/$--toolchainName--$.h\" \
-DZA_GENERATED_HEADER=\"app/builder/$--deviceName--$/$--deviceName--$.h\" \
$--dashDMacrosMak:"\"--$ \

CINC = -I./ \
-I$(ARM_IAR7_DIR)/ARM/INC \
-I$--stackDirFromProjBs:\/--$ \
-I$--frameworkDirFromProjBs:\/--$ \
-I$--frameworkDirFromProjBs:\/--$/../.. \
-I$--frameworkDirFromProjBs:\/--$/../../stack \
-I$--frameworkDirFromProjBs:\/--$/../util \
-I$--frameworkDirFromProjBs:\/--$/include \
-I$--halDirFromProjBs:\/--$ \
-I$--halDirFromProjBs:\/--$/plugin \
-I$--halDirFromProjBs:\/--$/.. \
-I$--halDirFromProjBs:\/--$/micro/cortexm3/efm32 \
-I$--halDirFromProjBs:\/--$/micro/cortexm3/efm32/config \
-I$--halDirFromProjBs:\/--$/micro/cortexm3/efm32/efr32 \
-I$--halDirFromProjBs:\/--$/../../$--cmsisDir:\/--$/Include \
-I$--halDirFromProjBs:\/--$/../../$--deviceDir:\/--$/SiliconLabs/$--chipFamily:\/--$/Include \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/common/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/config \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/dmadrv/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/gpiointerrupt/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/rtcdrv/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/sleep/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/spidrv/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/tempdrv/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/uartdrv/inc \
-I$--halDirFromProjBs:\/--$/../../$--emdrvDir:\/--$/ustimer/inc \
-I$--halDirFromProjBs:\/--$/../../$--emlibDir:\/--$/inc \
-I$--halDirFromProjBs:\/--$/../../middleware/$--glibDir:\/--$ \
-I$--halDirFromProjBs:\/--$/../../middleware/$--glibDir:\/--$/glib \
-I$--halDirFromProjBs:\/--$/../../radio/$--railLibDir:\/--$/plugin \
-I$--halDirFromProjBs:\/--$/../../radio/$--railLibDir:\/--$/chip/efr32/rf/common/cortex \
-I$--halDirFromProjBs:\/--$/../../radio/$--railLibDir:\/--$/chip/efr32/rf/rfprotocol/ieee802154/cortex \
-I$--stackDirFromProjBs:\/--$/platform/halconfig/inc/hal-config \
-I$--stackDirFromProjBs:\/--$/hardware/kit/common/halconfig \
-I$--stackDirFromProjBs:\/--$/hardware/kit/common/bsp \
$--includePathsMak--$ \

TARGET = $--deviceName--$

CSOURCES = $(filter %.c, $(SOURCE_FILES))
ASMSOURCES = $(filter %.s79, $(SOURCE_FILES))
ASMSOURCES2 = $(filter %.s, $(SOURCE_FILES))

COBJS = $(addprefix $(OUTPUT_DIR)/,$(CSOURCES:.c=.o))
ASMOBJS = $(addprefix $(OUTPUT_DIR)/,$(ASMSOURCES:.s79=.o))
ASMOBJS2 = $(addprefix $(OUTPUT_DIR)/,$(ASMSOURCES2:.s=.o))

OUTPUT_DIRS = $(sort $(dir $(COBJS)) $(dir $(ASMOBJS)) $(dir $(ASMOBJS2)))

ARCHITECTUREID = $--architectureIdLowerCase--$

# GNU ARM compiler
ifeq ($(findstring +gcc,$(ARCHITECTUREID)), +gcc)
$(info GCC Build)
	# Add linker circular reference as the order of objects may matter for any libraries used
	GROUP_START =-Wl,--start-group
	GROUP_END =-Wl,--end-group        

	CCFLAGS = -g3 \
    -gdwarf-2 \
    -mcpu=cortex-m4 \
    -mthumb \
    -std=gnu99 \
    -Os  \
    -Wall  \
    -c  \
    -fmessage-length=0  \
    -ffunction-sections  \
    -fdata-sections  \
    -mfpu=fpv4-sp-d16  \
    -mfloat-abi=softfp \
	$(CDEFS) \
	$(CINC) \

	ASMFLAGS = -c \
	-g3 \
	-gdwarf-2 \
	-mcpu=cortex-m4 \
	-mthumb \
	-c \
	-x assembler-with-cpp \
	$(CINC) \
	$(ASMDEFS)

	LDFLAGS = -g3 \
	-gdwarf-2 \
	-mcpu=cortex-m4 \
	-mthumb -T "$(GLOBAL_BASE_DIR)/hal/micro/cortexm3/efm32/gcc-cfg.ld" \
	-L"$(GLOBAL_BASE_DIR)/hal/micro/cortexm3/" \
	-Xlinker --defsym="SIMEEPROM_SIZE=$--simeepromSize--$" \
	-Xlinker --defsym="FLASH_SIZE=$--flashSize--$" \
	-Xlinker --defsym="RAM_SIZE=$--ramSize--$" \
	-Xlinker --defsym="APP_BTL=1" \
	-Xlinker --defsym="EMBER_MALLOC_HEAP_SIZE=$--mallocHeapSize--$" \
	-Xlinker --gc-sections \
	-Xlinker -Map="$(PROJECTNAME).map" \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=softfp --specs=nano.specs -u _printf_float \
	-Wl,--start-group -lgcc -lc -lnosys   -Wl,--end-group

	ARCHFLAGS = r

	ELFTOOLFLAGS_BIN = -O binary
	ELFTOOLFLAGS_HEX = -O ihex
	ELFTOOLFLAGS_S37 = -O srec

	ifeq ($(OS),Windows_NT)
		ARCH = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc-ar.exe
		AS = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc.exe
		CC = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc.exe
		ELFTOOL = $(ARM_GNU_DIR)/bin/arm-none-eabi-objcopy.exe
		LD = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc.exe
	else
		ARCH = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc-ar
		AS = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc
		CC = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc
		ELFTOOL = $(ARM_GNU_DIR)/bin/arm-none-eabi-objcopy
		LD = $(ARM_GNU_DIR)/bin/arm-none-eabi-gcc
	endif

endif

# IAR 7.xx toolchain
ifeq ($(findstring +iar,$(ARCHITECTUREID)), +iar)
$(info IAR Build)

	# IAR is not sensitive to linker lib order thus no groups needed.
	GROUP_START =
	GROUP_END =
	CINC += -I$(ARM_IAR6_DIR)/ARM/INC

	ifndef ARM_IAR7_DIR
	$(error ARM_IAR7_DIR is not set. Please define ARM_IAR7_DIR pointing to your IAR 7.xx installation folder.)
	endif

	CCFLAGS = --cpu=cortex-m3 \
	--cpu_mode=thumb \
	--diag_suppress=Pa050 \
	-e \
	--endian=little \
	--fpu=none \
	--no_path_in_file_macros \
	--separate_cluster_for_initialized_variables \
	--dlib_config=$(ARM_IAR7_DIR)/ARM/inc/c/DLib_Config_Normal.h \
	--debug \
	--dependencies=m $*.d \
	-Ohz \
	$(CDEFS) \
	$(CINC)

	ASMFLAGS = --cpu cortex-M3 \
	--fpu None \
	-s+ \
	"-M<>" \
	-w+ \
	-t2 \
	-r \
	$(CINC) \
	$(ASMDEFS)

	LDFLAGS = --entry __iar_program_start \
	--diag_suppress=Lp012 \
	--config $(GLOBAL_BASE_DIR)/hal/micro/cortexm3/efm32/iar-cfg.icf \
	--config_def $--bootloader--$=1 \
	--config_def EMBER_MALLOC_HEAP_SIZE=$--mallocHeapSize--$

	ARCH = $(JAMEXE_PREFIX) $(ARM_IAR7_DIR)/arm/bin/iarchive.exe
	AS = $(JAMEXE_PREFIX) $(ARM_IAR7_DIR)/arm/bin/iasmarm.exe
	CC = $(JAMEXE_PREFIX) $(ARM_IAR7_DIR)/arm/bin/iccarm.exe
	ELFTOOL = $(JAMEXE_PREFIX) $(ARM_IAR7_DIR)/arm/bin/ielftool.exe
	LD = $(JAMEXE_PREFIX) $(ARM_IAR7_DIR)/arm/bin/ilinkarm.exe

	# No archiver arguments needed for IAR.
	ARCHFLAGS =

	ELFTOOLFLAGS_BIN = --bin
	ELFTOOLFLAGS_HEX = --ihex
	ELFTOOLFLAGS_S37 = --srec --srec-s3only

endif

.PHONY: all clean PROLOGUE

all: PROLOGUE $(OUTPUT_DIRS) $(COBJS) $(ASMOBJS) $(ASMOBJS2) $(LIB_FILES)
	@echo 'Linking...'
	@$(LD) $(GROUP_START) $(LDFLAGS) $(COBJS) $(ASMOBJS) $(ASMOBJS2) $(LIB_FILES) $(GROUP_END) -o $(OUTPUT_DIR)/$(TARGET).out	
	@$(ELFTOOL) $(OUTPUT_DIR)/$(TARGET).out $(ELFTOOLFLAGS_BIN) $(OUTPUT_DIR)/$(TARGET).bin
	@$(ELFTOOL) $(OUTPUT_DIR)/$(TARGET).out $(ELFTOOLFLAGS_HEX) $(OUTPUT_DIR)/$(TARGET).hex
	@$(ELFTOOL) $(OUTPUT_DIR)/$(TARGET).out $(ELFTOOLFLAGS_S37) $(OUTPUT_DIR)/$(TARGET).s37
	@echo 'Done.'

PROLOGUE:
#	@echo $(COBJS)
#	@echo $(ASMOBJS)
#	@echo $(ASMOBJS2)

$(OUTPUT_DIRS):
	@mkdir -p $@

$(COBJS): %.o:
	@echo 'Building $(notdir $(@:%.o=%.c))...'
	@$(CC) $(CCFLAGS) -o $@ $(filter %$(@:$(OUTPUT_DIR)/%.o=%.c),$(CSOURCES)) > /dev/null \

$(ASMOBJS): %.o:
	@echo 'Building $(notdir $(@:%.o=%.s79))...'
	@$(AS) $(ASMFLAGS) -o $@ $(filter %$(@:$(OUTPUT_DIR)/%.o=%.s79),$(ASMSOURCES)) > /dev/null

$(ASMOBJS2): %.o:
	@echo 'Building $(notdir $(@:%.o=%.s))...'
	@$(AS) $(ASMFLAGS) -o $@ $(filter %$(@:$(OUTPUT_DIR)/%.o=%.s),$(ASMSOURCES2)) > /dev/null
  
clean:
	$(RM) -r $(COBJS)
	$(RM) -r $(ASMOBJS)
	$(RM) -r $(ASMOBJS2)
	$(RM) -r $(OUTPUT_DIR)
