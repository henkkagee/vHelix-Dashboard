# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\vHelix_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\vHelix_autogen.dir\\ParseCache.txt"
  "vHelix_autogen"
  )
endif()
