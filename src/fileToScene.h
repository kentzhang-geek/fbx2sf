//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_FILETOSCENE_H
#define RPD_FBX_PLUGIN_FILETOSCENE_H

#include <string>
#include "scene.h"

using namespace sf;

std::unique_ptr<SceneT> fileToFBSScene(std::string filename, flatbuffers::FlatBufferBuilder & builder);

#endif //RPD_FBX_PLUGIN_FILETOSCENE_H
