add_executable(MCoreSystem MCoreSystem.cxx)
target_link_libraries(MCoreSystem cysim cycommon pthread ${NVEC} ${IDA})
