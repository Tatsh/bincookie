include(GNUInstallDirs)

set(GCC_CLANG_SHARED_C_FLAGS "-fno-builtin;-std=gnu11")
set(GCC_CLANG_DEBUG_C_FLAGS "-ggdb;\
-O0;\
-pedantic;\
-Wall;\
-Wdeprecated-declarations;\
-Wempty-body;\
-Werror;\
-Werror=conversion;\
-Werror=float-conversion;\
-Werror=implicit-function-declaration;\
-Werror=incompatible-pointer-types;\
-Werror=int-conversion;\
-Werror=return-type;\
-Werror=sign-conversion;\
-Wmissing-braces;\
-Wmissing-field-initializers;\
-Wno-missing-prototypes;\
-Wno-strict-prototypes;\
-Wno-trigraphs;\
-Wno-unknown-pragmas;\
-Wparentheses;\
-Wpointer-sign;\
-Wshadow;\
-Wsign-compare;\
-Wswitch;\
-Wuninitialized;\
-Wunknown-pragmas;\
-Wunreachable-code;\
-Wunused-function;\
-Wunused-label;\
-Wunused-parameter;\
-Wunused-value;\
-Wunused-variable")

add_executable(bincookie2netscape convert2netscape.c)
if(CMAKE_COMPILER_IS_GNUCC OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  target_compile_options(bincookie2netscape
                         PRIVATE ${GCC_CLANG_SHARED_C_FLAGS}
                                 $<$<CONFIG:Debug>:${GCC_CLANG_DEBUG_C_FLAGS}>)
endif()
target_link_libraries(bincookie2netscape bincookie)

install(TARGETS bincookie2netscape DESTINATION ${CMAKE_INSTALL_BINDIR})
