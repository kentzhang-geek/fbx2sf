//
// Created by Admin on 2022/6/18.
//

#include "interface.h"
#include "fileToScene.h"

static flatbuffers::FlatBufferBuilder sta_builder;

extern "C" char *fileToSceneBuffer(char *filename, int pathlen, int *size_len) {
    std::string filepath(filename, pathlen);
    printf("get file %s\n", filepath.c_str());
    fflush(stdout);
    sta_builder = flatbuffers::FlatBufferBuilder();
    auto scene = fileToFBSScene(filepath, sta_builder);
    sta_builder.Finish(scene);
    *size_len = sta_builder.GetSize();

    return (char *) (sta_builder.GetBufferPointer());
}

void printHello() {
    printf("Hello!\n");
}
