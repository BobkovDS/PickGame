cmake_minimum_required( VERSION 3.18 )

set(MY_VERSION 0.1)


project(${FBXProjectName}
VERSION 0.1)

make_sources(SRL_LIST)

# 1 add_library(${FBXProjectName} ${SRL_LIST})
add_executable(${FBXProjectName} WIN32 ${SRL_LIST})

set_target_properties(${FBXProjectName} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../library)

# 2 target_link_libraries(${ProjectName} PUBLIC "library\\FbxSDK\\lib\\vs2017\\x64\\debug" )
target_link_libraries(${FBXProjectName} PUBLIC 
${PROJECT_BINARY_DIR}/library/FbxSDK/lib/vs2017/x64/debug/libfbxsdk-md.lib  
${PROJECT_BINARY_DIR}/library/FbxSDK/lib/vs2017/x64/debug/libxml2-md.lib 
${PROJECT_BINARY_DIR}/library/FbxSDK/lib/vs2017/x64/debug/zlib-md.lib)

include_directories("library\\FbxSDK\\include")

target_include_directories(${FBXProjectName} PUBLIC "${PROJECT_BINARY_DIR}")