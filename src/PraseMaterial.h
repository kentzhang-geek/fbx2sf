//
// Created by administrator on 9/30/2023.
//

#ifndef RPD_FBX_PLUGIN_PRASEMATERIAL_H
#define RPD_FBX_PLUGIN_PRASEMATERIAL_H


#include "scene.h"
#include "bvh_accel.h"
#include "material.h"
#include "fbxsdk.h"

void ParsePhongSurface(FbxSurfacePhong *phong, PhongSurfaceT *surface);
void ParseLambertSurface(FbxSurfaceLambert *lambert, LambertSurfaceT * surface);

std::unique_ptr<TextureT> ParseTexture(FbxFileTexture * tex);

std::unique_ptr<MaterialT> ParseMaterial(FbxSurfaceMaterial * m);


#endif //RPD_FBX_PLUGIN_PRASEMATERIAL_H
