DESCRIPTION = "tool to show how long a CPU or cluster enters idle state"
SUMMARY = "Idlestat is a tool which can show how long a CPU or cluster \
enters idle state. This infomation is obtained using traces from trace-cmd \
or ftrace tools."
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=eb723b61539feef013de476e68b5c50a"
SRCREV = "2247a8e07d208470b381b90e0540ee3cced3fbcb"
PV = "0.8+git${SRCPV}"

SRC_URI = "git://git.linaro.org/power/idlestat.git \
           file://Makefile.patch \
"

S = "${WORKDIR}/git"

do_compile() {
    oe_runmake CC='${CC}'
}

do_install () {
    install -D -p -m0755 idlestat ${D}/${sbindir}/idlestat
}
