do_install_prepend() {
    sed -i '/^dhcp-range=/d' ${WORKDIR}/dnsmasq.conf
}
