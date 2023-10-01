cmake_minimum_required(VERSION 3.0)
project(fbx2sf)
# include_directories(scene_flat/generated/cpp) # for deploy compiling
include_directories(scene_flat_dev/generated/cpp) # for dev compiling

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
        src/fileToScene.cpp src/fileToScene.h src/ParseNode.cpp src/ParseNode.h src/ParseMesh.cpp src/ParseMesh.h
        src/interface.cpp src/interface.h src/macro.h src/sceneToFile.cpp src/sceneToFile.h
        src/PraseMaterial.cpp
        src/PraseMaterial.h)

add_executable(test ${SRC} src/test.cpp)
add_executable(fbx2sf ${SRC} src/main.cpp
        src/tool.h)
add_library(fbxplugin ${SRC})
