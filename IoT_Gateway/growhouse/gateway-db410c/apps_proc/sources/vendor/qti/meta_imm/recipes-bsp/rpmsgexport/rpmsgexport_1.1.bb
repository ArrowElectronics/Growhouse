DESCRIPTION = "rpmsgexport implements RPMSG_CREATE_EPT_IOCTL for usage in udev rules to automatically create endpoint devices as remoteproc devices are booted"


LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://../git/LICENSE;md5=ff273e1fd41fa52668171e0817c89724"


SRC_URI = "git://github.com/andersson/rpmsgexport.git;protocol=https;branch=master;name=rpmsgexport"
SRCREV = "master"
SRC_URI[md5sum] = "153ebd9eda850f8dda7aa9c4726aa51f"
SRC_URI[sha256sum] = "6afd3f292ee9c7941923df5acd4a20dfaaf91fd57290ef307bfba45a7e4a8a1d"


S = "${WORKDIR}/git"

do_install () {
    oe_runmake install DESTDIR=${D} prefix=${prefix}
}
