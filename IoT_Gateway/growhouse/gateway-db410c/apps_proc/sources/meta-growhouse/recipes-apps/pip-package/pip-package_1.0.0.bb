##############################################################
#    pip-package_1.0.0.bb: bitbake recipe for python package #
#    Copyright(C) 2018                                       #
##############################################################

SUMMARY = "Install required packages of python to support ncp firmware upgrade"
LICENSE = "CLOSED"

# relative path
SRC_URI = "file://pyserial-3.4.dist-info/ \
           file://serial \
           file://xmodem \
           file://xmodem-0.4.5-py2.7.egg-info \
           "

do_compile() {
	true
}

do_install() {
	install -d ${D}/usr/lib64/python2.7/site-packages/
	cp -r ${WORKDIR}/pyserial-3.4.dist-info ${D}/usr/lib64/python2.7/site-packages/
	cp -r ${WORKDIR}/serial ${D}/usr/lib64/python2.7/site-packages/
	cp -r ${WORKDIR}/xmodem ${D}/usr/lib64/python2.7/site-packages/
	cp -r ${WORKDIR}/xmodem-0.4.5-py2.7.egg-info ${D}/usr/lib64/python2.7/site-packages/
}

FILES_${PN} += "/usr/*"
