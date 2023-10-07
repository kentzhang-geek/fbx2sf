//
// Created by administrator on 10/6/2023.
//

#ifndef RPD_FBX_PLUGIN_PARSELIGHT_H
#define RPD_FBX_PLUGIN_PARSELIGHT_H

#include "light.h"
#include "fbxsdk.h"

std::unique_ptr<LightT> ParseLight(FbxLight *light, FbxNode *node);
// TODO no dump for now

#endif //RPD_FBX_PLUGIN_PARSELIGHT_H
