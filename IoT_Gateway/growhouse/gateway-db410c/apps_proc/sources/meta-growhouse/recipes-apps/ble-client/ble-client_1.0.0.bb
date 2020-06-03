#########################################################
#    ble-server_1.0.0.bb: bitbake recipe for ble-server #
#    Copyright(C) 2018                                  #
#########################################################

SUMMARY = "BLE Client app for transreceiver testing"
LICENSE = "CLOSED"

SRC_URI = "file://ble-client file://prescan_test_prerequisite.sh"

S = "${WORKDIR}"

inherit systemd

DEPENDS += " cjson mosquitto readline bluez5 "

FILES_${PN} = "/usr/sbin/"
FILES_${PN} += "/etc/"

INSANE_SKIP_${PN} = "ldflags"

do_compile() {
	cd ${S}/ble-client
	oe_runmake BUILD_PATH_OE=${TOPDIR}
}

do_install() {
	install -d ${D}/usr/sbin/
	install -m 0755 ${S}/ble-client/blegatt-client ${D}/usr/sbin/ble-client

	install -d ${D}${sysconfdir}/
	install -m 0755 ${S}/prescan_test_prerequisite.sh ${D}${sysconfdir}/prescan_test_prerequisite.sh
}
