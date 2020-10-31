# ---------------------------------------------------------------------------------
#                           Common definitons
# ---------------------------------------------------------------------------------
.PHONY: all clean

CFLAGS = -g -O3 -Iinc -fPIC -fno-short-enums -D_DEBUG -DARM_ARCH_7 -DUSE_SYSLOG
LDFLAGS = -ldl -lpthread -lm -shared
TARGET := libadsprpc.so

# ---------------------------------------------------------------------------------
#                       Make the shared library (libadsprpc)
# ---------------------------------------------------------------------------------

SOURCES = \
	src/fastrpc_apps_user.c \
	src/remotectl_stub.c \
	src/listener.c \
	src/adsp_current_process_stub.c \
	src/apps_std_skel.c \
	src/apps_std_imp.c \
	src/apps_mem_imp.c \
	src/apps_mem_skel.c \
	src/rpcmem.c \
	src/apps_remotectl_skel.c \
	src/std.c \
	src/std_mem.c \
	src/std_path.c \
	src/std_dtoa.c \
	src/std_strlprintf.c \
	src/BufBound.c \
	src/std_SwapBytes.c \
	src/smath.c \
	src/atomic.c \
	src/cae.c

OBJFILES := $(patsubst %.c,%.o,$(SOURCES))
DEPFILES := $(patsubst %.o,%.d,$(OBJFILES))

include $(dep)

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) ${LDFLAGS} -o $@ $^

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.c.o:
	@echo "(CC) $@"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJFILES) $(DEPFILES) $(TARGET)
