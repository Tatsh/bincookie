project(libbinarycookies)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(GNUInstallDirs)
set(BINCOOKIE_VERSION 0.0.2)
add_library(bincookie SHARED bincookie.c bincookie.h)
set_target_properties(bincookie PROPERTIES PUBLIC_HEADER bincookie.h)
set_target_properties(bincookie
                      PROPERTIES VERSION
                                 ${BINCOOKIE_VERSION}
                                 SOVERSION
                                 0)

if(APPLE AND BUILD_FRAMEWORK)
  set_target_properties(bincookie
                        PROPERTIES FRAMEWORK
                                   TRUE
                                   FRAMEWORK_VERSION
                                   A
                                   MACOSX_FRAMEWORK_IDENTIFIER
                                   sh.tat.bincookie
                                   MACOSX_FRAMEWORK_SHORT_VERSION_STRING
                                   ${BINCOOKIE_VERSION}
                                   MACOSX_FRAMEWORK_BUNDLE_VERSION
                                   ${BINCOOKIE_VERSION}
                                   PUBLIC_HEADER
                                   bincookie.h)
endif()

install(TARGETS bincookie
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_PREFIX}/Library/Frameworks)
if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()
