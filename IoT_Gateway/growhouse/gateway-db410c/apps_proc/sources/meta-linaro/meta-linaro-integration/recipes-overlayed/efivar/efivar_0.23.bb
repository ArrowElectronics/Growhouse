SUMMARY = "Tools to manipulate UEFI variables"
DESCRIPTION = "efivar provides a simple command line interface to the UEFI variable facility"
HOMEPAGE = "https://github.com/rhinstaller/efivar"

LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://COPYING;md5=6626bb1e20189cfa95f2c508ba286393"

DEPENDS_class-target = "popt efivar-native"

inherit pkgconfig

SRCREV = "7367d78281fa3ce390e10c7c9f4608c58e7a6d3d"
SRC_URI = "git://github.com/rhinstaller/efivar.git"
SRC_URI_append_class-target = " file://0001-efivar-fix-for-cross-compile.patch \
                              "
S = "${WORKDIR}/git"

# Setting CROSS_COMPILE breaks pkgconfig, so just set AR
EXTRA_OEMAKE = "AR=${TARGET_PREFIX}gcc-ar"

do_compile_prepend() {
    sed -i -e s:-Werror::g ${S}/gcc.specs
}

do_compile_class-native() {
    oe_runmake -C src makeguids
}

# parallel install breaks with missing .pc files
PARALLEL_MAKEINST = "-j1"

do_install() {
    if ! [ -e ${S}/src/efivar.pc ] ; then
        sed -e "s,@@VERSION@@,${PV},g" -e "s,@@LIBDIR@@,${libdir},g" ${S}/src/efivar.pc.in > ${S}/src/efivar.pc
        sed -e "s,@@VERSION@@,${PV},g" -e "s,@@LIBDIR@@,${libdir},g" ${S}/src/efiboot.pc.in > ${S}/src/efiboot.pc
    fi
    oe_runmake install DESTDIR=${D}
}

do_install_class-native() {
    install -D -m 0755 ${B}/src/makeguids ${D}${bindir}/makeguids
}

BBCLASSEXTEND = "native"

