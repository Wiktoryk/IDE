add_library(ide-util STATIC util.cpp)
target_include_directories(ide-util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-util PUBLIC Qt6::Core)
