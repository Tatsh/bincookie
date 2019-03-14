project(bincookie)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

include(GNUInstallDirs)

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

if(WITH_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc ALL
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    # install(DIRECTORY )
  endif()
endif()

if(WITH_TESTS AND NOT APPLE)
  find_library(CMOCKA_LIBRARY cmocka)
  find_file(CMOCKA_HEADER cmocka.h HINTS /opt/local/include)
  get_filename_component(CMOCKA_INCLUDE_DIR ${CMOCKA_HEADER} DIRECTORY)
  set(mocked_funcs "fopen;fread;fseek;ftell;rewind;fclose")
  set(test_flags "-fprofile-arcs;-ftest-coverage")
  foreach(func ${mocked_funcs})
    list(APPEND test_flags "-Wl,--wrap,${func}")
  endforeach()
  add_executable(test test.c)
  target_include_directories(test PRIVATE ${CMOCKA_INCLUDE_DIR})
  target_link_libraries(test ${CMOCKA_LIBRARY} ${test_flags})
endif()
