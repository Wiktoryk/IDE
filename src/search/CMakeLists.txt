add_library(ide-search STATIC ripgrep_runner.cpp)

target_include_directories(ide-search PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-search PUBLIC Qt6::Core)

if (MSVC)
  target_compile_options(ide-search PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-search PRIVATE -Wno-system-headers)
endif()
