prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: hangdetector
Description: The hangdetector library
Version: @HANGDETECTOR_VERSION@
Cflags: -I${includedir}
Libs: -L${libdir} -lhangdetector
