project(libbinarycookies)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

include(GNUInstallDirs)

set(BINCOOKIE_VERSION 0.0.2)
add_library(bincookie SHARED bincookie.c bincookie.h)

set(SHARED_C_FLAGS "-fno-builtin -std=gnu11")
set(DEBUG_C_FLAGS "-ggdb -O0 \
-pedantic \
-Wall \
-Wdeprecated-declarations \
-Wempty-body \
-Werror \
-Werror=conversion \
-Werror=float-conversion \
-Werror=implicit-function-declaration \
-Werror=incompatible-pointer-types \
-Werror=int-conversion \
-Werror=return-type \
-Werror=sign-conversion \
-Wmissing-braces \
-Wmissing-field-initializers \
-Wno-missing-prototypes \
-Wno-strict-prototypes \
-Wno-trigraphs \
-Wno-unknown-pragmas \
-Wparentheses \
-Wpointer-sign \
-Wshadow \
-Wsign-compare \
-Wswitch \
-Wuninitialized \
-Wunknown-pragmas \
-Wunreachable-code \
-Wunused-function \
-Wunused-label \
-Wunused-parameter \
-Wunused-value \
-Wunused-variable")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SHARED_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_C_FLAGS}")

set_target_properties(bincookie
                      PROPERTIES PUBLIC_HEADER
                                 bincookie.h
                                 VERSION
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
