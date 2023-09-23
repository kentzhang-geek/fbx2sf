//
// Created by Admin on 2022/6/18.
//
#include "fileToScene.h"
#include "interface.h"

int main() {
    int datasize;
    std::string path = "samplefbx/source/scene.fbx";
    char * buffer = fbxFileToSceneBuffer((char *)path.c_str(), path.size(), &datasize);
    printf("datasize is %d\n", datasize);
    char * bufout = (char*)malloc(datasize);
    memcpy(bufout, buffer, datasize);
    std::string fpath = "outtest.fbx";
    sceneBufferToFbxFile((char *)fpath.c_str(), fpath.size(), bufout, datasize);

    return 0;
}
