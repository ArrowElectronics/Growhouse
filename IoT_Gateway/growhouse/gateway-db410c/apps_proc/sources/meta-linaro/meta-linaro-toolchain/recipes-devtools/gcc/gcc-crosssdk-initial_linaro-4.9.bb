require gcc-linaro-common-4.9.inc
require recipes-devtools/gcc/gcc-cross-initial_${PV}.bb
require recipes-devtools/gcc/gcc-crosssdk-initial.inc

EXTRA_OECONF += " --with-native-system-header-dir=${SYSTEMHEADERS} "
