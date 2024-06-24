#########################################################
#    ble-server_1.0.0.bb: bitbake recipe for ble-server #
#    Copyright(C) 2018                                  #
#########################################################

SUMMARY = "BLE server application which talks to mobile application via ble"
LICENSE = "CLOSED"

SRC_URI = "file://ble-server-simulator/ file://ble-server file://ble-server.service"

S = "${WORKDIR}"

inherit systemd

DEPENDS += " cjson mosquitto readline bluez5 "

FILES_${PN} = "/usr/sbin/"

INSANE_SKIP_${PN} = "ldflags"

do_compile() {
	cd ble-server-simulator
	oe_runmake

	cd ${S}/ble-server
	oe_runmake BUILD_PATH_OE=${TOPDIR}
}

do_install() {
	install -d ${D}/usr/sbin/
	install -m 0755 ${S}/ble-server-simulator/bleServer ${D}/usr/sbin/ble-server-simulator
	install -m 0755 ${S}/ble-server-simulator/bleApp ${D}/usr/sbin/ble-simulator-app
	install -m 0755 ${S}/ble-server/blegatt-server ${D}/usr/sbin/ble-server

	install -d ${D}${sysconfdir}/systemd/system
	install -m 0644 ${S}/ble-server.service ${D}${sysconfdir}/systemd/system/
}

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "ble-server.service"
SYSTEMD_AUTO_ENABLE = "enable"
