SUMMARY = "Update Public Bluetooth MAC address from device tree"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=8b9255257198f5ad2006cfdf0a1f24c1"

inherit externalsrc

COMPATIBLE_MACHINE = "dragonboard-410c"
EXTERNALSRC = "${TOPDIR}/../sources/vendor/qti/wcnss-bt-mac-update"

inherit systemd allarch

do_install () {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/wcnss-bt-mac-update.service ${D}${systemd_unitdir}/system
}

RDEPENDS_${PN} += "bluez5 bluez5-noinst-tools"

SYSTEMD_SERVICE_${PN} = "wcnss-bt-mac-update.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"
