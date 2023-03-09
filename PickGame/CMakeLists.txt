cmake_minimum_required( VERSION 3.18 )

set(MY_VERSION 0.1)
#set(CPPPATH packages\\Microsoft.Windows.CppWinRT.2.0.201017.1\\build\\native\\Microsoft.Windows.CppWinRT.targets)
set(PIXPATH packages\\WinPixEventRuntime.1.0.230302001\\build\\WinPixEventRuntime.targets)
project(${PickProjectName}
VERSION 0.1)

make_sources(SRL_LIST)

add_executable(${PickProjectName} WIN32 ${SRL_LIST})

#configure_file("config\\Directory.Build.props" "Directory.Build.props" COPYONLY)
configure_file("config\\packages.config" "packages.config" COPYONLY)
file(COPY "Shaders" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}  )

set_property(TARGET ${PickProjectName}
    PROPERTY VS_PROJECT_IMPORT "${PROJECT_BINARY_DIR}\\${PIXPATH}")
	
target_include_directories(${PickProjectName} 
PUBLIC "${PROJECT_BINARY_DIR}")