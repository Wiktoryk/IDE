add_library(ide-search STATIC ripgrep_runner.cpp)
target_include_directories(ide-search PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-search PUBLIC Qt6::Core)
