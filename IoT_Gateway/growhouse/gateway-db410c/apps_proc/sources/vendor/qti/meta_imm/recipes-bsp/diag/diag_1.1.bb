DESCRIPTION = "Diag for MSM platforms"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f6832ae4af693c6f31ffd931e25ef580"

inherit externalsrc

EXTERNALSRC = "${TOPDIR}/../sources/vendor/qti/diag"

COMPATIBLE_MACHINE = "(dragonboard-410c|dragonboard-820c)"

do_install () {
    install -d ${D}/${prefix}/bin
    oe_runmake -C ${S} install DESTDIR=${D} prefix=${prefix}
    install -d ${D}/${sysconfdir}/diag
    install -m 0644 ${S}/conf.temp ${D}/${sysconfdir}/diag
    install -d ${D}/${sysconfdir}/udev/rules.d
    install -m 0644 ${S}/rpmsg.rules ${D}/${sysconfdir}/udev/rules.d
}

CFLAGS += "-I. -I./FTM_WLAN -I./FTM_WLAN/libtcmd -I./FTM_WLAN/wlan_nv -I${STAGING_INCDIR}/libnl3"
CFLAGS += "-DLIBNL_2 -DWLAN_API_NL80211 -DCONFIG_FTM_WLAN "
CFLAGS += "-DDEBUG "
CFLAGS += "-fPIC "
EXTRA_OEMAKE = "CFLAGS='${CFLAGS} `pkg-config --cflags libnl-3.0`'"

LDFLAGS += "-L. -l:libpthread.so -l:libdl.so "
LDFLAGS += "-l:libnl-3.so -l:libnl-genl-3.so "
LDFLAGS += "-l:librt.so "
EXTRA_OEMAKE += "LDFLAGS='${LDFLAGS}'"

do_clean () {
    oe_runmake -C ${S} clean DESTDIR=${D} prefix=${prefix}
}

DEPENDS += "libnl"
RDEPENDS_${PN} += "udev"

FILES_${PN} += "${prefix}/lib/ftm_bt.so"
FILES_${PN} += "${prefix}/lib/ftm_wlan.so"

