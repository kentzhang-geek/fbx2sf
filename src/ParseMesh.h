//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_PARSEMESH_H
#define RPD_FBX_PLUGIN_PARSEMESH_H

#include "scene.h"
#include "bvh_accel.h"
#include "fbxsdk.h"

using namespace sf;

std::unique_ptr<MeshPrimitiveT> ParseMesh(FbxMesh *mesh);
bool DumpMesh(FbxMesh * mesh, const MeshPrimitive * mp, FbxManager * fbxsdkMan, FbxNode *node);


#endif //RPD_FBX_PLUGIN_PARSEMESH_H
