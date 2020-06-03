DESCRIPTION = "OpenDataPlane (ODP) provides a data plane application programming \
	environment that is easy to use, high performance, and portable between networking SoCs."
HOMEPAGE = "http://www.opendataplane.org"
SECTION = "console/network"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=4ccfa994aa96974cfcd39a59faee20a2"

DEPENDS = "openssl cunit libpcap"

SRC_URI = "git://git.linaro.org/lng/odp.git;name=odp"

SRCREV_odp = "693cabe510094121dcac58fed2672b6c67320f7d"
SRCREV_FORMAT = "odp"

S = "${WORKDIR}/git"

inherit autotools ptest

PACKAGECONFIG[perf] = "--enable-test-perf,,,"

do_configure_ptest() {
	oe_runconf --enable-test-vald --with-testdir=${PTEST_PATH}/test/
}

do_install_ptest() {
	oe_runmake DESTDIR=${D} install
	ln -s test/run-test ${D}/${PTEST_PATH}/run-ptest
}

#do_install_append() {
#	find ${D} -name "*.la" -delete
#}

PACKAGES =+ "${PN}-bin-dbg ${PN}-helper-dbg ${PN}-ptest-dbg ${PN}-helper-staticdev ${PN}-bin ${PN}-helper ${PN}-helper-dev"
# ODP is primarily shipped as static library plus some API test and samples/
FILES_${PN}-staticdev += "${libdir}/libodp-linux.a"
FILES_${PN}-helper-staticdev += "${libdir}/libodphelper-linux.a"
FILES_${PN}-helper-dev = " \
	${includedir}/odp/helper/* \
	${libdir}/libodphelper-linux.la \
	${libdir}/libodphelper-linux.so \
	${libdir}/pkgconfig/libodphelper-linux* \
"
FILES_${PN}-bin = " \
	${bindir}/* \
"
FILES_${PN}-bin-dbg = " \
	${bindir}/.debug/* \
	${exec_prefix}/src/debug/${PN}/${PV}-${PR}/git/example/* \
"
FILES_${PN}-dev = " \
	${includedir}/odp.h \
	${includedir}/odp_api.h \
	${includedir}/odp/api/* \
	${libdir}/libodp-linux.la \
	${libdir}/libodp-linux.so \
	${libdir}/pkgconfig/libodp-linux.* \
"
FILES_${PN}-helper += " \
	${libdir}/libodphelper-linux.so.* \
"
FILES_${PN} += " \
	${libdir}/libodp-linux.so.* \
"
FILES_${PN}-ptest-dbg = " \
	${exec_prefix}/src/debug/${PN}/${PV}-${PR}/git/test/* \
	${libdir}/odp/ptest/test/.debug/* \
"
FILES_${PN}-helper-dbg = " \
	${libdir}/.debug/libodphelper-linux.so* \
	${exec_prefix}/src/debug/${PN}/${PV}-${PR}/git/helper/* \
"
FILES_${PN}-dbg = " \
	${libdir}/.debug/libodp-linux.so* \
	${exec_prefix}/src/debug/${PN}/${PV}-${PR}/git/include/* \
	${exec_prefix}/src/debug/${PN}/${PV}-${PR}/git/platform/* \
"

RDEPENDS_${PN} = "bash libcrypto"
RDEPENDS_${PN}-ptest = "bash cunit"
