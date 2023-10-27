//
// Created by administrator on 9/30/2023.
//

#include "PraseMaterial.h"
#include "tool.h"
#include <unordered_set>

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

static std::unordered_set<std::string> texture_cache;

std::unique_ptr<TextureT> ParseTexture(FbxFileTexture *tex) {
    std::unique_ptr<TextureT> ret(new TextureT());
    ret->id = tex->GetUniqueID();
    ret->file_name = tex->GetFileName();
    ret->image_type = ImageType_eFileData;
    ret->media_name = std::string(tex->GetMediaName().Buffer(), tex->GetMediaName().Size());
    ReadFileContent(ret->file_name, ret->data);
    if (ret->data.empty())
    {
        RTP_LOG("Empty texture?");
    }
    if (texture_cache.count(ret->file_name) == 0) {
        texture_cache.insert(ret->file_name);
    } else {
        RTP_LOG("NOTE: Texture file duplicated", ret->file_name);
    }
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

FbxSurfaceMaterial *DumpMaterial(const Material *m, FbxScene *pScene) {
    switch (m->surface_type()) {
        case Surface_LambertSurface: {
            FbxSurfaceLambert *ret = FbxSurfaceLambert::Create(pScene, "LambertSurface");
            DumpLambert(m->surface_as_LambertSurface(), pScene, ret);
            return ret;
            break;
        }
        case Surface_PhongSurface: {
            FbxSurfacePhong * ret = FbxSurfacePhong::Create(pScene, "PhongSurface");
            DumpPhong(m->surface_as_PhongSurface(), pScene, ret);
            return ret;
            break;
        }
    }
    return nullptr;
}

void DumpLambert(const LambertSurface * lambert, FbxScene *pScene, FbxSurfaceLambert *ret) {
    ret->Diffuse.Set(FbxDouble3(lambert->diffuse()->x(), lambert->diffuse()->y(), lambert->diffuse()->z()));
    if (lambert->diffuse_texture()) {
        ret->Diffuse.ConnectSrcObject(DumpTexture(lambert->diffuse_texture(), pScene));
    }
    ret->Ambient.Set(FbxDouble3(lambert->ambient()->x(), lambert->ambient()->y(), lambert->ambient()->z()));
    if (lambert->ambient_texture()) {
        ret->Ambient.ConnectSrcObject(DumpTexture(lambert->ambient_texture(), pScene));
    }
    ret->NormalMap.Set(FbxDouble3(lambert->normal()->x(), lambert->normal()->y(), lambert->normal()->z()));
    if (lambert->normal_texture()) {
        ret->NormalMap.ConnectSrcObject(DumpTexture(lambert->normal_texture(), pScene));
    }
    return;
}

void DumpPhong(const PhongSurface * phong, FbxScene *pScene, FbxSurfacePhong *ret) {
    ret->Specular.Set(FbxDouble3(phong->specular()->x(), phong->specular()->y(), phong->specular()->z()));
    if (phong->specular_texture()) {
        ret->Specular.ConnectSrcObject(DumpTexture(phong->specular_texture(), pScene));
    }
    DumpLambert(phong->base(), pScene, ret);
}

FbxFileTexture * DumpTexture(const Texture *tex, FbxScene *pScene) {
    FbxFileTexture * ret = FbxFileTexture::Create(pScene, "Texture");
    static FbxTexture::ETextureUse textureUseMap[] = {FbxTexture::ETextureUse::eStandard};
    ret->SetFileName(tex->file_name()->c_str());
    ret->SetTextureUse(textureUseMap[0]);
    ret->SetMaterialUse(FbxFileTexture::eModelMaterial);
    ret->SetMappingType(fbxsdk::FbxTexture::eUV);
    ret->SetSwapUV(false);
    ret->SetTranslation(0.0, 0.0);
    ret->SetScale(1.0, 1.0);
    ret->SetRotation(0.0, 0.0);
    // TODO embed media from scene flat
    return ret;
}
