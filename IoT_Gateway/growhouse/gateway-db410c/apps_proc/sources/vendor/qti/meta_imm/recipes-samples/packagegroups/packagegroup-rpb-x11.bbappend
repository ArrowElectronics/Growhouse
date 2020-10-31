RDEPENDS_packagegroup-rpb-x11 += " \
    ${@bb.utils.contains("LICENSE_FLAGS_WHITELIST", "commercial_gstreamer1.0-plugins-ugly", "gstreamer1.0-plugins-ugly", "", d)} \
    gstreamer1.0-rtsp-server \
    kmscube \
    packagegroup-fonts-truetype \
    xrandr \
"
