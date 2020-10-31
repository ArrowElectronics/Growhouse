SUMMARY = "A pure lib-unixsocket-java file transfer into rootfs "
LICENSE = "CLOSED"

SRC_URI = "file://java/* \
	file://jni/* "
	
S = "${WORKDIR}"

FILES_${PN} = "${datadir}/java/unix*.jar"
FILES_${PN} += "${libdir}/jni/libunix-java.so"

# oe_runmake require do_compile to be defined
do_compile() {
    true
    echo "************do_compile****************"
}

do_install () {

  install -d ${D}${datadir}/java
  install -m 0644 ${WORKDIR}/java/unix*.jar ${D}${datadir}/java
  ln -sf unix-0.5.jar ${D}${datadir}/java/unix.jar
  install -d ${D}${libdir}/jni
  install -m 0655 ${WORKDIR}/jni/libunix-java.so ${D}${libdir}/jni
}

