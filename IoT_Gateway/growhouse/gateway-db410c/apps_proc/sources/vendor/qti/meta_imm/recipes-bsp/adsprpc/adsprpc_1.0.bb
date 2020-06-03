DESCRIPTION = "ADSP RPC Library"
HOMEPAGE = ""
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=e868d7526728e9b1c0c4f34730d27754"
COMPATIBLE_MACHINE = "(dragonboard-410c|dragonboard-820c)"

#SRC_URI = ""

inherit externalsrc
EXTERNALSRC = "${TOPDIR}/../sources/vendor/qti/adsprpc"

INSANE_SKIP_${PN} = "ldflags"
FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/libadsprpc.so"

do_compile () {
    oe_runmake -C ${S}
}

do_install () {
    install -d ${D}${libdir}
    install -m 0644 ${S}/libadsprpc.so ${D}${libdir}/libadsprpc.so
}
