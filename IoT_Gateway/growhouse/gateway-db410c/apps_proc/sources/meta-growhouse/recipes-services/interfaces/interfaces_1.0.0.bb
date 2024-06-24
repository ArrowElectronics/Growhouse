########################################################
#    Copyright(C) 2018 eInfochips All rights reserved.
########################################################
SUMMARY = "Add interfaces file for network"
LICENSE = "CLOSED"

SRC_URI = "file://interfaces"

S = "${WORKDIR}"

FILES_${PN} = "/etc/"

do_install() {
	install -d ${D}${sysconfdir}/network

	install -m 0755 ${S}/interfaces ${D}${sysconfdir}/network
}
