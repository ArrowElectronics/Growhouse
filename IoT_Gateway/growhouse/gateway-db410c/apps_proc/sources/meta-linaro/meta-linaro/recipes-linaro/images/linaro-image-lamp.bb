require linaro-image-common.inc

IMAGE_INSTALL += " \
    apache2 \
    grub \
    mysql5-server \
    mysql5-client \
    php-fpm \
    php-fpm-apache2 \
    packagegroup-core-buildessential \
    wget \
    ${SDK_IMAGE_INSTALL}"

# Grub doesn't build for armv7a/hf
IMAGE_INSTALL_remove_armv7a = "grub"

IMAGE_FEATURES += "\
	dev-pkgs \
	staticdev-pkgs \
	tools-debug \
	tools-sdk \
	"
