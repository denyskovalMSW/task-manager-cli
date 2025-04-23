add_library(services ReminderService.cpp LoggerService.cpp HintService.cpp AutoSaveService.cpp)
target_include_directories(services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Залежність від utils, бо Task.cpp використовує DateTimeUtils
target_link_libraries(services PUBLIC utils cli)