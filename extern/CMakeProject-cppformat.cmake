list(APPEND CPPFORMAT_SRC
  "cppformat/format.cc"
)

list(APPEND CPPFORMAT_HPP
  "cppformat/format.h"
)

source_group("" FILES ${CPPFORMAT_SRC} ${CPPFORMAT_HPP})

add_library("cppformat" ${CPPFORMAT_SRC} ${CPPFORMAT_HPP})

set_property(TARGET "cppformat" PROPERTY FOLDER "External Libraries")

disable_project_warnings("cppformat")

target_include_directories("cppformat" PUBLIC "cppformat")

if (MSVC)
  sm_add_compile_definition("cppformat" _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
  set_target_properties("cppformat" PROPERTIES
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "gnu++14"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
  )
else() # Unix
  sm_add_compile_flag("cppformat" "-std=gnu++11")
  if (CMAKE_CXX_COMPILER MATCHES "clang")
    sm_add_compile_flag("cppformat" "-stdlib=libc++")
  endif()
endif()

