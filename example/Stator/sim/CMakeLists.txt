add_executable(Stator Stator.cxx)
target_link_libraries(Stator cysim cycommon pthread ${NVEC} ${IDA})
