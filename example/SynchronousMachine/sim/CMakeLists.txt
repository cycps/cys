add_executable(TurbineHouse TurbineHouse.cxx)
target_link_libraries(TurbineHouse cysim cycommon pthread ${NVEC} ${IDA})
