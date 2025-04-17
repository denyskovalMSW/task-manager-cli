add_library(cli CommandParser.cpp UI.cpp)
target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Припустимо, CommandParser або UI залежить від core
target_link_libraries(cli PUBLIC core)