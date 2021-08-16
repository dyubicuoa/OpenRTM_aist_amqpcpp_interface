CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(amqp_communication_interface CXX)
message("Project name is: ${PROJECT_NAME}")

set(PROJECT_VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

add_subdirectory(src)
add_subdirectory(tests)
