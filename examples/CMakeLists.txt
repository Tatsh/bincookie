include(GNUInstallDirs)
add_executable(bincookie2netscape convert2netscape.c)
target_link_libraries(bincookie2netscape bincookie)
install(TARGETS bincookie2netscape
        DESTINATION ${CMAKE_INSTALL_BINDIR})
