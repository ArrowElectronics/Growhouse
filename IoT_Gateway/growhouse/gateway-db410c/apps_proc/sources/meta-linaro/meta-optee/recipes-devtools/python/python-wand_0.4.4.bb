DESCRIPTION = "Magic wand library"
HOMEPAGE = "http://docs.wand-py.org/en/0.4.4/"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=170eafd687d4a2b950819cd5e067e6d5"

SRCNAME = "wand"

SRCREV = "c731c620c3c96c409a194dafab396ffbb97d6702"
SRC_URI = "git://github.com/dahlia/wand.git"
S = "${WORKDIR}/git"

inherit setuptools

# Requires imagemagick-6 while meta-oe morty and newer provides imagemagick-7 which isn't compatible with wand
#
# You can import imagemagick-5 from older meta-oe, before this upgrade:
#    commit ec660639ecea3fcb6e554b6f1bafc504e8f2fc78
#    Author: Randy MacLeod <randy.macleod@windriver.com>
#    Date:   Mon Aug 8 17:41:34 2016 -0400
#    imagemagick: upgrade from 6.9.2 to 7.0.2
# and add this commit on top of that:
#    commit dfcb67af35936a351789044039a55e3fad299c1a
#    Author: Andreas Müller <schnitzeltony@googlemail.com>
#    Date:   Sun Sep 18 02:47:26 2016 +0200
#    imagemagick: depend on fftw not virtual/fftw
#
# We need this old version becase python-wand-native used here
# depends on older imagemagick-native as discussed here:
# https://stackoverflow.com/questions/37011291/python-wand-image-is-not-recognized
# there still isn't newer python-wand supporting 7.* version:
# https://github.com/dahlia/wand/blob/4fe2c6ba9cb0d4105361cea6e9e9e83116080473/wand/api.py#L58

DEPENDS += " imagemagick-native"

BBCLASSEXTEND = "native"

FILES_${PN} += "${datadir}"
