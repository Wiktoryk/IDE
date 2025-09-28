add_executable(ide main.cpp)
set_target_properties(ide PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
)
target_link_libraries(ide PRIVATE ide-ui ide-util ide-buffer ide-pty ide-search ide-git Qt6::Widgets Qt6::Gui Qt6::Core)
set_target_properties(ide PROPERTIES WIN32_EXECUTABLE FALSE MACOSX_BUNDLE FALSE)

if (MSVC)
  target_compile_options(ide PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide PRIVATE -Wno-system-headers)
endif()