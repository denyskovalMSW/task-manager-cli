add_library(io JsonStorage.cpp)
target_include_directories(io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Залежність від core, бо JsonStorage використовує Task
target_link_libraries(io PUBLIC core)