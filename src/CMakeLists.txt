add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(cli)
add_subdirectory(app)

add_executable(task_manager main.cpp)
target_link_libraries(task_manager PRIVATE app cli io core utils)