#########################################################################
#    watchdogDaemon_1.0.0.bb: Recipe for growhose-server and service    #
#    Copyright(C) 2018                                                  #
#########################################################################

SUMMARY = "Watchdog daemon to keep petting the hardware watchdog"
LICENSE = "CLOSED"

SRC_URI = "file://watchdog_daemon.c file://Makefile"

S = "${WORKDIR}"

FILES_${PN} = "/usr/sbin/"

INSANE_SKIP_${PN} = "ldflags"

do_compile() {
	make LINKER_FLAGS=${LDFLAGS}
}

do_install() {
	install -d ${D}/usr/sbin/
	install -m 0755 ${S}/watchdogDaemon ${D}/usr/sbin/
}

