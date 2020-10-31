########################################################
#    Copyright(C) 2018 eInfochips All rights reserved.
########################################################
SUMMARY = "Post boot init scripts"
LICENSE = "CLOSED"

SRC_URI = "file://rc.local file://run_zigbee-coordinator.sh \
           file://ncp.py \
           file://ncp_fw_upgrade.sh \
           file://factoryreset.sh \
           file://gatewayDiag.sh \
           "

S = "${WORKDIR}"

FILES_${PN} = "/etc/"

do_install() {
	install -d ${D}${sysconfdir}/zigbee-coordinator
	install -d ${D}${sysconfdir}/ncp-fw-upgrade
	install -d ${D}${sysconfdir}/factoryreset
	install -d ${D}${sysconfdir}/gatewayDiag

	install -m 0755 ${S}/rc.local ${D}${sysconfdir}/
	install -m 0755 ${S}/run_zigbee-coordinator.sh ${D}${sysconfdir}/zigbee-coordinator
	install -m 0755 ${S}/ncp.py ${D}${sysconfdir}/ncp-fw-upgrade/
	install -m 0755 ${S}/ncp_fw_upgrade.sh ${D}${sysconfdir}/ncp-fw-upgrade/
	install -m 0755 ${S}/factoryreset.sh ${D}${sysconfdir}/factoryreset/
	install -m 0755 ${S}/gatewayDiag.sh ${D}${sysconfdir}/gatewayDiag/
}
