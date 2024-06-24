BASEPV = "5.2"
PV = "linaro-${BASEPV}"
require recipes-devtools/gcc/gcc-cross_${PV}.bb
require recipes-devtools/gcc/gcc-cross-initial.inc
