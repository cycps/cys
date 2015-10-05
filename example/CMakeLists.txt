find_library(NVEC NAMES libsundials_nvecserial.a)
find_library(IDA NAMES libsundials_ida.a)

add_subdirectory(RotorSystem)
add_subdirectory(MagneticCore)
add_subdirectory(SynchronousMachine)
