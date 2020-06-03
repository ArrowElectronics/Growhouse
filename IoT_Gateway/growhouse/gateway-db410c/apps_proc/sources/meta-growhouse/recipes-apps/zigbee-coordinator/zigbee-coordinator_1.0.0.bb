#####################################################################
#    zigbee-coordinator_1.0.0.bb: Recipe for zigbee-coordinator     #
#    Copyright(C) 2018 eInfochips All rights reserved.              #
#####################################################################

SUMMARY = "connect to internet for gateway"
LICENSE = "CLOSED"

SRC_URI = "file://Z3GatewayHost/ file://zigbee-coordinator.service"

inherit systemd

S = "${WORKDIR}"

DEPENDS += " readline "

FILES_${PN} = "/usr/sbin/"

SDK_DIR = "${TOPDIR}/../../../end-devices/gecko_sdk_suite/v2.3/"

INSANE_SKIP_${PN} = "ldflags"

do_compile() {
	cd Z3GatewayHost
	make clean
	make SDK_PATH_GW=${SDK_DIR} COMPILER_ARCH64="${CC}" AR_ARCH64=${AR}
}

do_install() {
	install -d ${D}/usr/sbin/
	install -m 0755 ${S}/Z3GatewayHost/build/exe/Z3GatewayHost ${D}/usr/sbin/zigbeeCoordinator

	install -d ${D}${sysconfdir}/systemd/system
	install -m 0644 ${S}/zigbee-coordinator.service ${D}${sysconfdir}/systemd/system/
}

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "zigbee-coordinator.service"
SYSTEMD_AUTO_ENABLE = "enable"
