########################################################
#    Copyright(C) 2018 eInfochips All rights reserved.
########################################################
SUMMARY = "cjson library required for json parsing"
LICENSE = "CLOSED"

SRC_URI = "file://cJSON/"

S = "${WORKDIR}"

FILES_${PN} = "/usr/lib64/"

do_compile() {
	cd ${S}/cJSON
	oe_runmake
}

do_install() {
	install -d ${D}${libdir}
	install -d ${D}${includedir}

	for l in ${S}/cJSON/*.so.*; do
		echo "install $l"
		oe_soinstall $l ${D}${libdir}
	done

	install -m 0644 ${S}/cJSON/cJSON.h ${D}${includedir}/
}
