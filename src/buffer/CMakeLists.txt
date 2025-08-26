add_library(ide-buffer STATIC buffer.cpp)
target_include_directories(ide-buffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-buffer PUBLIC Qt6::Core)
