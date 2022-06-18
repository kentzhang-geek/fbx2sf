//
// Created by Admin on 2022/6/18.
//
#include "fileToScene.h"
#include "interface.h"

int main() {
    std::string buffer;
//    fileToSceneBuffer("samplefbx/source/scene.fbx", buffer);
    int size;
    std::string path = "samplefbx/source/scene.fbx";
    fileToSceneBuffer((char *)path.c_str(), path.size(), &size);
//    printf("size is %d\n", buffer.size());
    printf("size is %d\n", size);

    return 0;
}
