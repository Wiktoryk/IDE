add_library(ide-pty STATIC pty.cpp)

target_include_directories(ide-pty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-pty PUBLIC Qt6::Core)


if (MSVC)
  target_compile_options(ide-pty PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-pty PRIVATE -Wno-system-headers)
endif()