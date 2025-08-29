add_library(ide-util STATIC util.cpp)

target_include_directories(ide-util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-util PUBLIC Qt6::Core)

if (MSVC)
  target_compile_options(ide-util PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-util PRIVATE -Wno-system-headers)
endif()