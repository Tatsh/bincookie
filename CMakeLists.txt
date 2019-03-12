project(libbinarycookies)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

include(GNUInstallDirs)

set(BINCOOKIE_VERSION 0.0.2)
add_library(bincookie INTERFACE)
target_include_directories(
  bincookie
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>)

if(MSVC)
  # Disable warnings about secure CRT, but still enable compatible signature
  # versions http://stackoverflow.com/a/119752/374110
  add_definitions(/D_CRT_SECURE_NO_WARNINGS /D__builtin_bswap32=_byteswap_ulong)
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/bincookie.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  add_custom_target(doc ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
