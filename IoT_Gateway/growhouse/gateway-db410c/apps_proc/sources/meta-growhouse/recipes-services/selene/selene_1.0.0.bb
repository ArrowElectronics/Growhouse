########################################################
#    Copyright(C) 2018 eInfochips All rights reserved.
########################################################
SUMMARY = "selene installation recipe"
LICENSE = "CLOSED"

inherit systemd

SRC_URI = "file://selene/*"

S = "${WORKDIR}"

FILES_${PN} = "/opt/selene/"

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

do_install() {
	install -d ${D}/opt/selene
	install -d ${D}/opt/selene/bin
	install -d ${D}/opt/selene/config
	install -d ${D}/opt/selene/config/devices
	install -d ${D}/opt/selene/databus
	install -d ${D}/opt/selene/db
	install -d ${D}/opt/selene/javaScript
	install -d ${D}/opt/selene/lib
	install -d ${D}/opt/selene/log
	install -d ${D}/opt/selene/updates/backup
	install -d ${D}/opt/selene/updates/download
	install -d ${D}${sysconfdir}/systemd/system
	install -m 0755 ${S}/selene/bin/selene.service ${D}/opt/selene/bin/selene.service
	install -m 0755 ${S}/selene/config/devices/mqtt-router.properties ${D}/opt/selene/config/devices/mqtt-router.properties
	install -m 0755 ${S}/selene/config/devices/self.properties ${D}/opt/selene/config/devices/self.properties
	install -m 0755 ${S}/selene/config/devices/mqtt-router_registration.js ${D}/opt/selene/config/devices/mqtt-router_registration.js
	install -m 0755 ${S}/selene/config/devices/mqtt-router_state.js ${D}/opt/selene/config/devices/mqtt-router_state.js
	install -m 0755 ${S}/selene/config/log4j2.xml ${D}/opt/selene/config/log4j2.xml
	install -m 0755 ${S}/selene/config/selene.properties ${D}/opt/selene/config/selene.properties
	install -m 0755 ${S}/selene/javaScript/registrationJavaScriptModbus.js ${D}/opt/selene/javaScript/registrationJavaScriptModbus.js
	install -m 0755 ${S}/selene/lib/selene-engine.jar ${D}/opt/selene/lib/selene-engine.jar

	install -m 0644 ${WORKDIR}/selene/bin/selene.service ${D}${sysconfdir}/systemd/system
}

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "selene.service"
SYSTEMD_AUTO_ENABLE = "enable"
