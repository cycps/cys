add_executable(RotorSystem RotorSystem.cxx)
target_link_libraries(RotorSystem cysim cycommon pthread ${NVEC} ${IDA})
