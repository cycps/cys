find_library(YAML NAMES libyaml-cpp.a)

add_executable(RotorSpeedController RotorSpeedController.cxx)
target_link_libraries(RotorSpeedController cycontrol cycommon pthread ${YAML})

add_executable(SP SP.cxx)
target_link_libraries(SP cycontrol cycommon pthread ${YAML})

