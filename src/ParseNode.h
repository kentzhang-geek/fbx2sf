//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_PARSENODE_H
#define RPD_FBX_PLUGIN_PARSENODE_H


#include "scene.h"
#include "bvh_accel.h"
#include "fbxsdk.h"

flatbuffers::Offset<BVHNode> ParseNode(FbxNode * node, flatbuffers::FlatBufferBuilder & builder);


#endif //RPD_FBX_PLUGIN_PARSENODE_H
