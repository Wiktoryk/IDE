add_library(ide-git STATIC git_repo.cpp)
target_include_directories(ide-git PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} SYSTEM PUBLIC ${LIBGIT2_INCLUDE_DIR})
target_link_libraries(ide-git PUBLIC git2 Qt6::Core)

if (MSVC)
  target_compile_options(ide-git PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-git PRIVATE -Wno-system-headers)
endif()