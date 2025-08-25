cmake_minimum_required(VERSION 3.10)
project(Flow CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define include directories
include_directories(include)

# Server executable
add_executable(FlowServer
    src/server/main.cpp
    src/server/ChatServer.cpp
    src/user/UserManager.cpp
)

# Client executable
add_executable(FlowClient
    src/client/main.cpp
    src/client/ChatClient.cpp
)

# Link libraries
if (WIN32)
    target_link_libraries(FlowServer PRIVATE ws2_32)
    target_link_libraries(FlowClient PRIVATE ws2_32)
else()
    target_link_libraries(FlowServer PRIVATE pthread)
    target_link_libraries(FlowClient PRIVATE pthread)
endif()
