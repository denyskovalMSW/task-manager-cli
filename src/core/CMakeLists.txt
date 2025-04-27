add_library(core Task.cpp TaskManager.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Залежність від utils, бо Task.cpp використовує DateTimeUtils
target_link_libraries(core PUBLIC utils cli io)