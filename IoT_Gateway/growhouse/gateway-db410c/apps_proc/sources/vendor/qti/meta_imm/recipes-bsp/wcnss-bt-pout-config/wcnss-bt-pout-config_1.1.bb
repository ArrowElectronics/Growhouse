SUMMARY = "Configure Bluetooth Power Out Level to Class 2"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=80209e4ca0ac81578c117cf3e33eb3c0"

inherit externalsrc

COMPATIBLE_MACHINE = "dragonboard-410c"
EXTERNALSRC = "${TOPDIR}/../sources/vendor/qti/wcnss-bt-pout-config"

inherit systemd allarch

do_install () {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/wcnss-bt-pout-config.service ${D}${systemd_unitdir}/system
}

RDEPENDS_${PN} += "bluez5 bluez5-noinst-tools"

SYSTEMD_SERVICE_${PN} = "wcnss-bt-pout-config.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"
