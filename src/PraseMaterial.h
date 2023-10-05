//
// Created by administrator on 9/30/2023.
//

#ifndef RPD_FBX_PLUGIN_PRASEMATERIAL_H
#define RPD_FBX_PLUGIN_PRASEMATERIAL_H


#include "scene.h"
#include "bvh_accel.h"
#include "material.h"
#include "fbxsdk.h"
#include <unordered_map>

/**
 * Parse a PhongSurface from a FbxSurfacePhong
 * @param phong
 * @param surface
 */
void ParsePhongSurface(FbxSurfacePhong *phong, PhongSurfaceT *surface);

/**
 * Parse a LambertSurface from a FbxSurfaceLambert
 * @param lambert
 * @param surface
 */
void ParseLambertSurface(FbxSurfaceLambert *lambert, LambertSurfaceT * surface);

/**
 * Parse a texture from a FbxFileTexture
 * @param tex
 * @return
 */
std::unique_ptr<TextureT> ParseTexture(FbxFileTexture * tex);

/**
 * Parse a material from a FbxSurfaceMaterial
 * @param m
 * @return
 */
std::unique_ptr<MaterialT> ParseMaterial(FbxSurfaceMaterial * m);

/**
 * Dump a material to a FbxSurfaceMaterial
 * @tparam MaterialT
 * @param m
 * @param pScene
 * @return
 */
FbxSurfaceMaterial * DumpMaterial(const Material * m, FbxScene * pScene);

/**
 * Dump a Lambert surface
 * @param m
 * @param pScene
 * @param ret
 */
void DumpLambert(const LambertSurface * lambert, FbxScene *pScene, FbxSurfaceLambert *ret);

/**
 * Dump a Phong surface
 * @param m
 * @param pScene
 * @param ret
 */
void DumpPhong(const PhongSurface * phong, FbxScene *pScene, FbxSurfacePhong *ret);

/**
 * Dump a Phong surface
 * @param m
 * @param pScene
 * @param ret
 */
FbxFileTexture * DumpTexture(const Texture *tex, FbxScene *pScene);

#endif //RPD_FBX_PLUGIN_PRASEMATERIAL_H
