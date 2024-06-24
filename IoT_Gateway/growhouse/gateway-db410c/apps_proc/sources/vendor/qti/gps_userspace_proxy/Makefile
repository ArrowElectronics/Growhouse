OUT := gps_proxy

CFLAGS := -Wall -g -I../qrtr/lib
LDFLAGS := -L../qrtr -lqrtr

SRCS := qmi_gps.c qmi_tlv.c gps_proxy.c util.c
OBJS := $(SRCS:.c=.o)

$(OUT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

#%.c: %.qmi
#	qmic < $<

test: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT) $(OBJS)

