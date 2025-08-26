add_executable(ide main.cpp)
target_link_libraries(ide PRIVATE ide-ui ide-util ide-buffer ide-pty ide-search ide-git Qt6::Widgets Qt6::Gui Qt6::Core)
set_target_properties(ide PROPERTIES WIN32_EXECUTABLE FALSE MACOSX_BUNDLE FALSE)
