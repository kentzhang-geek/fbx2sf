//
// Created by administrator on 9/30/2023.
//

#include "PraseMaterial.h"
#include "tool.h"

std::unique_ptr<MaterialT> ParseMaterial(FbxSurfaceMaterial *m) {
    std::unique_ptr<MaterialT> ret(new MaterialT());
    ret->id = m->GetUniqueID();
    if (FbxCast<FbxSurfacePhong>(m)) {
        ret->surface.Set(PhongSurfaceT());
        ParsePhongSurface(FbxCast<FbxSurfacePhong>(m), ret->surface.AsPhongSurface());
    } else if (FbxCast<FbxSurfaceLambert>(m)) {
        ret->surface.Set(LambertSurfaceT());
        ParseLambertSurface(FbxCast<FbxSurfaceLambert>(m), ret->surface.AsLambertSurface());
    } else {
        RTP_LOG("Unknown material type", m->GetName());
    }
    return ret;
}

void ParsePhongSurface(FbxSurfacePhong *phong, PhongSurfaceT *surface) {
    // specular
    surface->specular = MakeVec3d(phong->Specular.Get().mData[0], phong->Specular.Get().mData[1], phong->Specular.Get().mData[2]);
    if (phong->Specular.GetSrcObject<FbxFileTexture>(0)) {
        surface->specular_texture = ParseTexture(phong->Specular.GetSrcObject<FbxFileTexture>(0));
    }
    // TODO reflection
    // Lambert basement
    surface->base = std::make_unique<LambertSurfaceT>();
    ParseLambertSurface(FbxCast<FbxSurfaceLambert>(phong), surface->base.get());
    return;
}

std::unique_ptr<TextureT> ParseTexture(FbxFileTexture *tex) {
    std::unique_ptr<TextureT> ret(new TextureT());
    ret->id = tex->GetUniqueID();
    ret->file_name = tex->GetFileName();
    ret->media_name = std::string(tex->GetMediaName().Buffer(), tex->GetMediaName().Size());
    ret->data = ReadFileContent(ret->file_name);
    return ret;
}

void ParseLambertSurface(FbxSurfaceLambert *lambert, LambertSurfaceT * surface) {
    surface->diffuse = MakeVec3d(lambert->Diffuse.Get().mData[0], lambert->Diffuse.Get().mData[1], lambert->Diffuse.Get().mData[2]);
    if (lambert->Diffuse.GetSrcObject<FbxFileTexture>(0)) {
        surface->diffuse_texture = ParseTexture(lambert->Diffuse.GetSrcObject<FbxFileTexture>(0));
    }
    surface->emissive = MakeVec3d(lambert->Emissive.Get().mData[0], lambert->Emissive.Get().mData[1], lambert->Emissive.Get().mData[2]);
    if (lambert->Emissive.GetSrcObject<FbxFileTexture>(0)) {
        surface->emissive_texture = ParseTexture(lambert->Emissive.GetSrcObject<FbxFileTexture>(0));
    }
    surface->ambient = MakeVec3d(lambert->Ambient.Get().mData[0], lambert->Ambient.Get().mData[1], lambert->Ambient.Get().mData[2]);
    if (lambert->Ambient.GetSrcObject<FbxFileTexture>(0)) {
        surface->ambient_texture = ParseTexture(lambert->Ambient.GetSrcObject<FbxFileTexture>(0));
    }
    surface->normal = MakeVec3d(lambert->NormalMap.Get().mData[0], lambert->NormalMap.Get().mData[1], lambert->NormalMap.Get().mData[2]);
    if (lambert->NormalMap.GetSrcObject<FbxFileTexture>(0)) {
        surface->normal_texture = ParseTexture(lambert->NormalMap.GetSrcObject<FbxFileTexture>(0));
    }
    return;
}
