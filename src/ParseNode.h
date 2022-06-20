//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_PARSENODE_H
#define RPD_FBX_PLUGIN_PARSENODE_H


#include "scene.h"
#include "bvh_accel.h"
#include "fbxsdk.h"

std::unique_ptr<BVHNodeT> ParseNode(FbxNode * node);
bool DumpNode(const BVHNode * node, FbxNode * fbxout, FbxManager * fbxsdkMan);


#endif //RPD_FBX_PLUGIN_PARSENODE_H
