add_library(app App.cpp)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Залежності, які App може використовувати
target_link_libraries(app PUBLIC cli io core utils)