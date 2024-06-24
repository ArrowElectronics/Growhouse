DIAG := qdiag
FTM_BT_OUT := ftm_bt.so
FTM_WLAN_OUT := ftm_wlan.so
TCMD_OUT := libtcmd.a

CFLAGS := -Wall -g -O2 -I. \
			-I./FTM_WLAN/libtcmd \
			-I./FTM_WLAN/wlan_nv \
			-DLIBNL_2 \
			-DWLAN_API_NL80211 \
#			-I$(LIBNL3_INCDIR) \
#			-I./FTM_WLAN/libnl \
#			-DUSE_GLIB \
#			-DUSE_DIAG_MALLOC \

LDFLAGS := -L. -l:libpthread.so -l:libdl.so -l:libnl-3.so -l:libnl-genl-3.so -l:libnl-route-3.so -l:librt.so

COMMON_SRC := hdlc.c

SRCS := Qdiag.c diag_cntl.c mbuf.c peripheral.c util.c watch.c qdiag_router.c qdiag_transport.c qdiag_config.c usb.c masks.c
FTM_BT_SRCS := ./FTM_BT/ftm_bt.c
TCMD_SRCS := ./FTM_WLAN/libtcmd/libtcmd.c ./FTM_WLAN/libtcmd/nl80211.c ./FTM_WLAN/libtcmd/os.c
FTM_WLAN_SRCS := ./FTM_WLAN/ftm_wlan.c ./FTM_WLAN/wlan_nv/wlan_nv.c ./FTM_WLAN/wlan_nv/wlan_nv_parser.c ./FTM_WLAN/wlan_nv/wlan_nv_stream_read.c ./FTM_WLAN/wlan_nv/wlan_nv_template_builtin.c

OBJS := $(SRCS:.c=.o)
COMMON_OBJS := $(COMMON_SRC:.c=.o)
FTM_BT_OBJS := $(FTM_BT_SRCS:.c=.o)
FTM_WLAN_OBJS := $(FTM_WLAN_SRCS:.c=.o)
TCMD_OBJS := $(TCMD_SRCS:.c=.o)

all: $(DIAG) $(FTM_BT_OUT) $(FTM_WLAN_OUT)

$(FTM_BT_OUT): $(FTM_BT_OBJS) $(COMMON_OBJS)
	$(CC) -shared -fPIC -o $@ $^ $(LDFLAGS)
	
$(TCMD_OUT): $(TCMD_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(AR) rsc -o $@ $^

$(FTM_WLAN_OUT): $(FTM_WLAN_OBJS) $(COMMON_OBJS) $(TCMD_OBJS)
	$(CC) -shared -fPIC -o $@ $^ $(LDFLAGS)

$(DIAG): $(OBJS) $(COMMON_OBJS)# $(TCMD_OBJS) $(FTM_WLAN_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

install: $(DIAG) $(FTM_BT_OUT) $(FTM_WLAN_OUT)
	install -D -m 755 $(DIAG) $(DESTDIR)$(prefix)/bin/$(DIAG)
	install -D -m 755 $(FTM_BT_OUT) $(DESTDIR)$(prefix)/lib/$(FTM_BT_OUT)
	install -D -m 755 $(FTM_WLAN_OUT) $(DESTDIR)$(prefix)/lib/$(FTM_WLAN_OUT)

clean:
	rm -f $(COMMON_OBJS)
	rm -f $(DIAG) $(OBJS) #$(TCMD_OBJS) $(FTM_WLAN_OBJS)
	rm -f $(FTM_BT_OUT) $(FTM_BT_OBJS)
	rm -f $(FTM_WLAN_OUT) $(FTM_WLAN_OBJS) $(TCMD_OBJS)
#	rm -f $(TCMD_OUT) $(TCMD_OBJS)
