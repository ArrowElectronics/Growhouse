
SRC= btgatt-client.c
INC= -I${BUILD_PATH_OE}/tmp-glibc/work/aarch64-linaro-linux/bluez5/5.48-r0/bluez-5.48/
OUT= blegatt-client
LIB=${BUILD_PATH_OE}/tmp-glibc/work/aarch64-linaro-linux/bluez5/5.48-r0/build/src/.libs/libshared-mainloop.a ${BUILD_PATH_OE}/tmp-glibc/work/aarch64-linaro-linux/bluez5/5.48-r0/build/lib/.libs/libbluetooth-internal.a ${BUILD_PATH_OE}/tmp-glibc/work/aarch64-linaro-linux/bluez5/5.48-r0/build/src/uuid-helper.o -lbluetooth -lreadline -lpthread -lrt -lmosquitto -lcjson

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(INC) -o $(OUT) $(SRC) $(LIB) -Wall -Werror
clean:
	rm -rf *.o
	rm -rf $(OUT)
