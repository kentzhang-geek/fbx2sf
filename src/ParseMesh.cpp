//
// Created by Admin on 2022/6/18.
//

#include "ParseMesh.h"
#include "tool.h"
#pragma optimize("", off)


template <typename InVecType>
void TemplPushArray(std::vector<Vec3d> &outarray, InVecType v) {
    outarray.push_back(Vec3d(v[0], v[1], v[2]));
}

template <typename InVecType>
void TemplPushArray(std::vector<Vec4d> &outarray, InVecType v) {
    outarray.push_back(Vec4d(v[0], v[1], v[2], v[3]));
}

template <typename InVecType>
void TemplPushArray(std::vector<Vec2d> &outarray, InVecType v) {
    outarray.push_back(Vec2d(v[0], v[1]));
}

template <typename AttributeArray, typename VecType>
void ExtractAttributesToSceneFlatMesh(AttributeArray * attri, std::vector<VecType> &mesh_attri, FbxMesh * mesh) {
    if (attri->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
        for (int i = 0; i < mesh->GetControlPointsCount(); ++i) {
            if (attri->GetReferenceMode() == fbxsdk::FbxLayerElement::eDirect) {
                auto v = attri->GetDirectArray().GetAt(i);
                TemplPushArray(mesh_attri, v);
            } else if (attri->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect) {
                auto v = attri->GetDirectArray().GetAt(attri->GetIndexArray().GetAt(i));
                TemplPushArray(mesh_attri, v);
            } else {
                printf("unsupported attribute reference mode detected\n");
            }
        }
    } else if (attri->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
        for (int i = 0; i < (mesh->GetPolygonCount() * 3); ++i) {
            if (attri->GetReferenceMode() == fbxsdk::FbxLayerElement::eDirect) {
                auto v = attri->GetDirectArray().GetAt(i);
                TemplPushArray(mesh_attri, v);
            } else if (attri->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect) {
                auto v = attri->GetDirectArray().GetAt(attri->GetIndexArray().GetAt(i));
                TemplPushArray(mesh_attri, v);
            } else {
                printf("unsupported color reference mode detected\n");
            }
        }
    } else {
        printf("unsupported attribute mapping mode detected?\n");
    }
}

template <typename LayerElementType>
void TemplatedAddVecForFbx(LayerElementType *layer, const Vec3d * v) {
    layer->GetDirectArray().Add(FbxVector4(v->x(), v->y(), v->z(), 0.0));
}

template <typename LayerElementType>
void TemplatedAddVecForFbx(LayerElementType *layer, const Vec2d * v) {
    layer->GetDirectArray().Add(FbxVector2(v->x(), v->y()));
}

template <typename VecType>
FbxGeometryElementUV * ConvertSFattributesToFbx(FbxMesh *owner, const MeshTexCoords *texCoords, FbxLayerElement::EType tType, char * uvname) {
    FbxGeometryElementUV* lLayerElementUV = owner->CreateElementUV(uvname, tType);
    lLayerElementUV->SetReferenceMode(fbxsdk::FbxLayerElement::eDirect);
    switch (texCoords->texCoordMapMode()) {
        case AttributeMapMode::AttributeMapMode_eByIndex : {
            lLayerElementUV->SetMappingMode(fbxsdk::FbxLayerElement::eByPolygonVertex);
            break;
        }
        case AttributeMapMode::AttributeMapMode_eByVertex : {
            lLayerElementUV->SetMappingMode(fbxsdk::FbxLayerElement::eByControlPoint);
            break;
        }
        default : {
            printf("unsolved diffuse tex coord mapping mode detected\n");
            break;
        }
    }
    for (int i = 0; i < texCoords->texCoords()->size(); i++) {
        auto n = texCoords->texCoords()->Get(i);
        TemplatedAddVecForFbx(lLayerElementUV,n);
    }
    return lLayerElementUV;
}


std::unique_ptr<MeshPrimitiveT> ParseMesh(FbxMesh *mesh) {
    std::unique_ptr<MeshPrimitiveT> ret = std::unique_ptr<MeshPrimitiveT>(new MeshPrimitiveT());
    // vertex
    std::vector<Vec3d> points;
    for (int i = 0; i < mesh->GetControlPointsCount(); ++i) {
        auto pt = mesh->GetControlPointAt(i);
        ret->vertexs.push_back(Vec3d(pt[0], pt[1], pt[2]));
    }
    // indexes
    std::vector<TriangleIndexTuple> triangleIndexes;
    for (int i = 0; i < mesh->GetPolygonCount(); ++i) {
        if (mesh->GetPolygonSize(i) == 3) {
            ret->triangles.push_back(TriangleIndexTuple(
                    mesh->GetPolygonVertex(i, 0),
                    mesh->GetPolygonVertex(i, 1),
                    mesh->GetPolygonVertex(i, 2)
            ));
        } else {
            // encounter non triangle polygon, this is not good
            printf("Encounter Non Triangle Polygon!\n");
        }
    }
    // tex coord for diffuse
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureDiffuse) >= 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureDiffuse);
        ret->diffuseTexCoord = std::make_unique<MeshTexCoordsT>();
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->diffuseTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->diffuseTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->diffuseTexCoord->texCoords, mesh);
    }
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureDiffuse) > 1) {
        // multiple tex coord detected
        printf("Multiple diffuse tex coord detected %d\n", mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureDiffuse));
    }
    // specular tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureSpecular) >= 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureSpecular);
        ret->specularTexCoord = std::make_unique<MeshTexCoordsT>();
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->specularTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->specularTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->specularTexCoord->texCoords, mesh);
    }
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureSpecular) > 1) {
        // multiple tex coord detected
        printf("Multiple specular tex coord detected %d\n", mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureSpecular));
    }
    // ambient tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureAmbient) >= 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureAmbient);
        ret->ambientTexCoord = std::make_unique<MeshTexCoordsT>();
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->ambientTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->ambientTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->ambientTexCoord->texCoords, mesh);
    }
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureAmbient) > 1) {
        // multiple tex coord detected
        printf("Multiple ambient tex coord detected %d\n", mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureAmbient));
    }
    // normal tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureNormalMap) >= 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureNormalMap);
        ret->normalTexCoord = std::make_unique<MeshTexCoordsT>();
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->normalTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->normalTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->normalTexCoord->texCoords, mesh);
    }
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureNormalMap) > 1) {
        // multiple tex coord detected
        printf("Multiple normal tex coord detected %d\n", mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureNormalMap));
    }
    // normal
    if (mesh->GetElementNormalCount() == 1) {
        FbxGeometryElementNormal *enor = mesh->GetElementNormal(0);
        if (enor->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->normalMapMode = AttributeMapMode_eByVertex;
        } else if (enor->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->normalMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported normal mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(enor, ret->normals, mesh);
    } else if (mesh->GetElementNormalCount() > 1) {
        // multiple normal detected
        printf("Multiple normal detected\n");
    }
    // parse material
    if (mesh->GetElementMaterialCount() == 1) {
        FbxLayerElementMaterial * meshMat = mesh->GetElementMaterial(0);
        if (meshMat->GetMappingMode() == fbxsdk::FbxLayerElement::eAllSame) {
            FbxSurfaceMaterial* lMaterial = mesh->GetNode()->GetMaterial(meshMat->GetIndexArray().GetAt(0));
            ret->materialIdx = lMaterial->GetUniqueID();
        } else {
            printf("unsupported mapping mode %d\n", meshMat->GetMappingMode()); // TODO : support other mapping mode
        }
    }
    if (mesh->GetElementMaterialCount() > 1) { // TODO : support multi material
        printf("unsupported material count: %d\n", mesh->GetElementMaterialCount());
    }
    return ret;
}

bool DumpMesh(FbxMesh *mesh, const MeshPrimitive *mp, FbxManager *fbxsdkMan, FbxNode *node) {
    if (mp->vertexs()) {
        mesh->InitControlPoints(mp->vertexs()->size());
        for (int i = 0; i < mp->vertexs()->size(); ++i) {
            auto vtx = mp->vertexs()->Get(i);
            FbxVector4 pt(vtx->x(), vtx->y(), vtx->z(), 1.0);
            mesh->SetControlPointAt(pt, i);
        }
    }
    // normal
    if (mp->normals()) {
        if (mesh->GetUVLayerCount() == 0)
            mesh->CreateLayer();
        FbxLayerElementNormal *lLayerElementNormal = FbxLayerElementNormal::Create(mesh, "normal");
        for (int i = 0; i < mp->normals()->size(); i++) {
            auto n = mp->normals()->Get(i);
            lLayerElementNormal->GetDirectArray().Add(FbxVector4(n->x(), n->y(), n->z(), 0.0));
        }
        switch (mp->normalMapMode()) {
            case AttributeMapMode::AttributeMapMode_eByIndex : {
                lLayerElementNormal->SetMappingMode(fbxsdk::FbxLayerElement::eByPolygonVertex);
                break;
            }
            case AttributeMapMode::AttributeMapMode_eByVertex : {
                lLayerElementNormal->SetMappingMode(fbxsdk::FbxLayerElement::eByControlPoint);
                break;
            }
            default : {
                printf("unsolved normal mapping mode detected\n");
                break;
            }
        }
        mesh->GetLayer(0)->SetNormals(lLayerElementNormal);
    }
    // triangle indices
    if (mp->triangles()) {
        for (int i = 0; i < mp->triangles()->size(); ++i) {
            mesh->BeginPolygon();
            auto tri = mp->triangles()->Get(i);
            mesh->AddPolygon(tri->idx0());
            mesh->AddPolygon(tri->idx1());
            mesh->AddPolygon(tri->idx2());
            mesh->EndPolygon();
        }
    }
    // diffuse tex coord
    if (mp->diffuseTexCoord()) {
        if (mesh->GetUVLayerCount() == 0)
            mesh->CreateLayer();
        FbxLayerElementUV *lLayerElementUV = ConvertSFattributesToFbx<FbxLayerElementUV>(mesh,mp->diffuseTexCoord(), fbxsdk::FbxLayerElement::eTextureDiffuse, "diffuseUV");
        mesh->GetLayer(0)->SetUVs(lLayerElementUV, fbxsdk::FbxLayerElement::eTextureDiffuse);
    }
    // ambient tex coord
    if (mp->ambientTexCoord()) {
        if (mesh->GetUVLayerCount() == 0)
            mesh->CreateLayer();
        FbxLayerElementUV *lLayerElementUV = ConvertSFattributesToFbx<FbxLayerElementUV>(mesh,mp->ambientTexCoord(), fbxsdk::FbxLayerElement::eTextureAmbient, "ambientUV");
        mesh->GetLayer(0)->SetUVs(lLayerElementUV, fbxsdk::FbxLayerElement::eTextureAmbient);
    }
    // specular tex coord
    if (mp->specularTexCoord()) {
        if (mesh->GetUVLayerCount() == 0)
            mesh->CreateLayer();
        FbxLayerElementUV *lLayerElementUV = ConvertSFattributesToFbx<FbxLayerElementUV>(mesh,mp->specularTexCoord(), fbxsdk::FbxLayerElement::eTextureSpecular, "specularUV");
        mesh->GetLayer(0)->SetUVs(lLayerElementUV, fbxsdk::FbxLayerElement::eTextureSpecular);
    }
    // material
    if (mp->materialIdx()) {
        FbxGeometryElementMaterial* lMaterialElement = mesh->CreateElementMaterial();
        lMaterialElement->SetMappingMode(FbxGeometryElement::eAllSame);
        lMaterialElement->SetReferenceMode(FbxGeometryElement::eDirect);
        lMaterialElement->GetIndexArray().Add(0);
        node->AddMaterial(MaterialMapUniqueID::get_const_instance().at(mp->materialIdx()));
    }
    return true;
}
