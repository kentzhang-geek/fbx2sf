//
// Created by Admin on 2022/6/18.
//

#include "interface.h"
#include "fileToScene.h"
#include "sceneToFile.h"


extern "C" char *fileToSceneBuffer(char *filename, int pathlen, int *size_len) {
    flatbuffers::FlatBufferBuilder builder;
    std::string filepath(filename, pathlen);
    printf("get file %s\n", filepath.c_str());
    fflush(stdout);
    auto scene = fileToFBSScene(filepath, builder);
    builder.Finish(Scene::Pack(builder, scene.get()));
    *size_len = builder.GetSize();
    void * ret_ptr = malloc(builder.GetSize());
    memcpy(ret_ptr, builder.GetBufferPointer(), builder.GetSize());

    return (char *)ret_ptr;
}

void printHello() {
    printf("Hello!\n");
}

bool sceneBufferToFile(char *filename, int pathlen, char *buffer, int size_len) {
    std::string filepath(filename, pathlen);
    printf("get file %s\n", filepath.c_str());
    flatbuffers::Verifier vr((uint8_t *)buffer, (size_t)size_len);
    printf("verify flatbuffers data input %d\n", GetScene(buffer)->Verify(vr));
    fflush(stdout);
    return sceneToFile(filepath, (Scene *) GetScene(buffer));
}
