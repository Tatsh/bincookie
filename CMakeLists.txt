project(libbinarycookies)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(GNUInstallDirs)
add_library(bincookie SHARED bincookie.c)
set_target_properties(bincookie PROPERTIES PUBLIC_HEADER bincookie.h)
set_target_properties(bincookie
                      PROPERTIES VERSION
                                 0.0.2
                                 SOVERSION
                                 0)
install(TARGETS bincookie
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION include)
if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()
