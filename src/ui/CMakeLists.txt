add_library(ide-ui STATIC mainwindow.cpp)
target_include_directories(ide-ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ide-ui PUBLIC Qt6::Widgets Qt6::Gui Qt6::Core)
