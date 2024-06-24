SUMMARY = "A pure Java D-Bus Bin and Jar file transfer into rootfs "
LICENSE = "CLOSED"

SRC_URI = "file://bin/* \
	file://java/* "
	
S = "${WORKDIR}"

FILES_${PN} = "${datadir}/java/*"
FILES_${PN} += "${bindir}/* "

# oe_runmake require do_compile to be defined
do_compile() {
    true
    echo "************do_compile****************"
}

do_install () {

  install -d ${D}${bindir}
  install -m 0644 ${WORKDIR}/bin/* ${D}/${bindir}
  install -d ${D}${datadir}/java
  install -m 0644 ${WORKDIR}/java/* ${D}${datadir}/java
  install -m 0644 ${WORKDIR}/java/dbus-bin.jar ${D}${datadir}/java
  ln -sf dbus-java-bin-2.7.jar ${D}${datadir}/java/dbus-bin.jar
  ln -sf libdbus-java-2.7.jar ${D}${datadir}/java/dbus.jar
  ln -sf dbus-java-bin-2.7.jar ${D}${datadir}/java/dbus-java-bin.jar
  ln -sf libdbus-java-2.7.jar ${D}${datadir}/java/dbus-java.jar
  ln -sf dbus-java-viewer-2.7.jar ${D}${datadir}/java/dbus-java-viewer.jar
  ln -sf dbus-java-viewer-2.7.jar ${D}${datadir}/java/dbus-viewer.jar
  ln -sf libdbus-java-2.7.jar ${D}${datadir}/java/libdbus-java.jar
}

