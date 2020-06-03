FILESPATH =+ "${TOPDIR}/../sources/vendor/qti/firmware-qcom-dragonboard410c:"
FILESEXTRAPATHS_prepend := "${THISDIR}:"

# In case we're dealing with local firmware file, override the SRC_URI
OVERRIDES_append = ":${LOCAL_FIRMWARE}"
SRC_URI_local = "file://${FIRMWARE_FILE}"

SRC_URI += "\
file://firmware-partition-update.service"

inherit systemd

do_install_append() {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/firmware-partition-update.service ${D}${systemd_unitdir}/system
}

SYSTEMD_SERVICE_${PN} = "firmware-partition-update.service"
SYSTEMD_AUTO_ENABLE_${PN} = "disable"
