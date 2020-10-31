SUMMARY = "Firmware testsuite"
DESCRIPTION = "The tool fwts comprises of over fifty tests that are designed to exercise and test different aspects of a machine's firmware. Many of these tests need super user access to read BIOS data and ACPI tables, so the tool requires running with super user privileges (e.g. with sudo)."
HOMEPAGE = "https://wiki.ubuntu.com/Kernel/Reference/fwts"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://src/main.c;beginline=1;endline=16;md5=31da590f3e9f3bd34dcdb9e4db568519"

PV = "16.12.00"

SRCREV = "d38fced7db86e5483d5c2e634ee1eec98620d5b8"
SRC_URI = "git://kernel.ubuntu.com/hwe/fwts.git \
           file://0001-ignore-constant-logical-operand-warning-with-clang.patch \
           "

S = "${WORKDIR}/git"

COMPATIBLE_HOST = "(i.86|x86_64|arm|aarch64|powerpc64).*-linux"

DEPENDS = "libpcre json-c glib-2.0 dtc"

inherit autotools pkgconfig

CFLAGS += "-I${STAGING_INCDIR}/json-c -Wno-error=unknown-pragmas"

FILES_${PN} += "${libdir}/fwts/lib*${SOLIBS}"
FILES_${PN}-dev += "${libdir}/fwts/lib*${SOLIBSDEV} ${libdir}/fwts/lib*.la"
FILES_${PN}-staticdev += "${libdir}/fwts/lib*a"
FILES_${PN}-dbg += "${libdir}/fwts/.debug"

TOOLCHAIN = "gcc"

