add_library(utils DateTimeUtils.cpp ActivityTracker.cpp)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})