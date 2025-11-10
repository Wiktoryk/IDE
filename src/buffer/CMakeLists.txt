add_library(ide-buffer STATIC textBuffer.h gapBuffer.h gapBuffer.cpp)

target_include_directories(ide-buffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-buffer PUBLIC Qt6::Core)


if (MSVC)
  target_compile_options(ide-buffer PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-buffer PRIVATE -Wno-system-headers)
endif()