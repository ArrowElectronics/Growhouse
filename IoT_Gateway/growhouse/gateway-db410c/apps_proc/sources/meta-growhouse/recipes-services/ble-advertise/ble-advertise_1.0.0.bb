###############################################################
#    ble-advertise_1.0.0.bb: bitbake recipe for ble-advertise #
#    Copyright(C) 2019 eInfochips All rights reerved          #
###############################################################

SUMMARY = "BLE Advertisement service"
LICENSE = "CLOSED"

SRC_URI = "file://ble-advertise.service file://ble-advertise.sh"

S = "${WORKDIR}"

inherit systemd

FILES_${PN} = "/etc/"

do_compile() {
}

do_install() {
	install -d ${D}${sysconfdir}/ble-advertise
	install -m 0755 ${S}/ble-advertise.sh ${D}${sysconfdir}/ble-advertise/

	install -d ${D}${sysconfdir}/systemd/system
	install -m 0644 ${S}/ble-advertise.service ${D}${sysconfdir}/systemd/system/
}

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "ble-advertise.service"
SYSTEMD_AUTO_ENABLE = "enable"
