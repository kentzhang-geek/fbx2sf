//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_PARSEMESH_H
#define RPD_FBX_PLUGIN_PARSEMESH_H

#include "scene.h"
#include "bvh_accel.h"
#include "fbxsdk.h"

flatbuffers::Offset<MeshPrimitive> ParseMesh(FbxMesh * mesh, flatbuffers::FlatBufferBuilder & builder);


#endif //RPD_FBX_PLUGIN_PARSEMESH_H
