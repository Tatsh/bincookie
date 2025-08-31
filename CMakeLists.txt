cmake_minimum_required(VERSION 3.31)
project(bincookie)

include(GNUInstallDirs)

add_library(bincookie INTERFACE)
target_include_directories(bincookie INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                               $<INSTALL_INTERFACE:include>)

if(MSVC)
  # Disable warnings about secure CRT, but still enable compatible signature versions
  # http://stackoverflow.com/a/119752/374110
  add_definitions(/D_CRT_SECURE_NO_WARNINGS /D__builtin_bswap32=_byteswap_ulong)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bincookie.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/bincookie.h.3
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/man/man3)

if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

if(WITH_DOCS)
  set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tags")
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  find_package(Doxygen REQUIRED COMPONENTS dot)
  doxygen_add_docs(docs README.md bincookie.h ALL)
endif()

if(WITH_TESTS AND CMAKE_COMPILER_IS_GNUCXX)
  enable_testing()
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(cmocka REQUIRED)
  set(mocked_funcs "fclose;fopen;fread;fseek;ftell;rewind")
  foreach(func ${mocked_funcs})
    list(APPEND test_flags "-Wl,--wrap,${func}")
  endforeach()
  add_executable(general_tests test.c)
  target_compile_options(general_tests PRIVATE --coverage)
  target_include_directories(general_tests PRIVATE ${CMOCKA_INCLUDE_DIR})
  target_link_options(general_tests PRIVATE ${test_flags})
  target_link_libraries(general_tests ${CMOCKA_LIBRARY} gcov)
  add_test(NAME general_tests COMMAND general_tests)
endif()
