cmake_minimum_required( VERSION 3.18 )

message(STATUS "Using CMake generator ${CMAKE_GENERATOR}")

set(PickProjectName PickGame)
set(FBXProjectName FBXLoader)

project(PickGameAppl)

add_definitions(-D_UNICODE)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ? set_property(GLOBAL PROPERTY USE_FOLDER ON)
# ? set_property(TARGET ${PickGameAppl} PROPERTY FOLDER "CMake projects")
#set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include Help module with some custom helpful function
include(help)

add_subdirectory(PickGame)

add_subdirectory(FBXLoader)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PickProjectName})

set_property(TARGET ${PickGameAppl}
    PROPERTY DOTNET_TARGET_FRAMEWORK_VERSION "v4.0"
)