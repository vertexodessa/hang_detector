prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: hangdetector
Description: The hangdetector library
Version: 0.1
Cflags: -I${includedir}
Libs: -L${libdir} -lhangdetector
