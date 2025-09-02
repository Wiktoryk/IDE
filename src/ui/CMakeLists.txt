file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(ide-ui STATIC)
target_sources(ide-ui PRIVATE ${UI_SOURCES})
target_include_directories(ide-ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-ui PUBLIC Qt6::Widgets Qt6::Gui Qt6::Core)

if (MSVC)
  target_compile_options(ide-ui PRIVATE /external:W0 /external:anglebrackets)
else()
  target_compile_options(ide-ui PRIVATE -Wno-system-headers)
endif()
