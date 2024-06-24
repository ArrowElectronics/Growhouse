SUMMARY = "Configure Alcatel x220d USB 3G modem to connect to the Pratner Israel carrier"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=8b9255257198f5ad2006cfdf0a1f24c1"

inherit externalsrc

EXTERNALSRC = "${TOPDIR}/../sources/vendor/qti/x220d-modem-config"

inherit systemd allarch

do_install () {
    install -d ${D}/${sysconfdir}
    install -m 0644 ${S}/usb-modeswitch-x220d.conf ${D}/${sysconfdir}
    install -m 0644 ${S}/wvdial.conf ${D}/${sysconfdir}
    install -d ${D}/${sysconfdir}/udev/rules.d
    install -m 0644 ${S}/50-x220d.rules ${D}/${sysconfdir}/udev/rules.d
}

RDEPENDS_${PN} += "usb-modeswitch udev"

#SYSTEMD_SERVICE_${PN} = "x220d-modem-config.service"
#SYSTEMD_AUTO_ENABLE_${PN} = "enable"
