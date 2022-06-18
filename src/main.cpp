//
// Created by Admin on 2022/6/18.
//
#include "fileToScene.h"

int main() {
    std::string buffer;
    fileToSceneBuffer("samplefbx/source/scene.fbx", buffer);
    printf("size is %d\n", buffer.size());

    return 0;
}
