cmake_minimum_required(VERSION 3.0)
project(rpd_fbx_plugin)
include_directories(fbs_generated)

# for flatbuffers
include_directories(SDK/flatbuffers/include)
link_directories(SDK/flatbuffers/lib)
link_libraries(flatbuffers)

# for FBX SDK
include_directories(SDK/2020.3.1/include/)
link_directories(SDK/2020.3.1/lib/vs2019/x64/release/)
link_libraries(libfbxsdk-md)
link_libraries(libxml2-md)
link_libraries(zlib-md)
#link_libraries(libfbxsdk)
#set(CMAKE_CXX_LINK_LIBRARY_FLAG  ${CMAKE_CXX_LINK_LIBRARY_FLAG} /NODEFAULTLIB:library)

set(SRC
        src/fileToScene.cpp src/fileToScene.h src/ParseNode.cpp src/ParseNode.h src/ParseMesh.cpp src/ParseMesh.h)

add_executable(test ${SRC} src/main.cpp src/interface.cpp src/interface.h src/macro.h src/sceneToFile.cpp src/sceneToFile.h)
add_library(${PROJECT_NAME} ${SRC} src/interface.cpp src/interface.h)