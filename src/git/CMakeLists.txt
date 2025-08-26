add_library(ide-git STATIC git_repo.cpp)
target_include_directories(ide-git PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-git PUBLIC git2 Qt6::Core)
