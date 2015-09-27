find_library(NVEC NAMES libsundials_nvecserial.a)
find_library(IDA NAMES libsundials_ida.a)

add_executable(RotorSystem RotorSystem.cxx)
target_link_libraries(RotorSystem cysim cycommon pthread ${NVEC} ${IDA})
