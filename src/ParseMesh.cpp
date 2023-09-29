//
// Created by Admin on 2022/6/18.
//

#include "ParseMesh.h"
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
    // colors
    if (mesh->GetElementVertexColorCount() == 1) {
        FbxGeometryElementVertexColor *ecolor = mesh->GetElementVertexColor(0);
        if (ecolor->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->colorMapMode = AttributeMapMode_eByVertex;
        } else if (ecolor->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->colorMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported color mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(ecolor, ret->colors, mesh);
    }
    // tex coord for diffuse
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureDiffuse) == 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureDiffuse);
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->diffuseTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->diffuseTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->diffuseTexCoord->texCoords, mesh);
    } else if (mesh->GetElementUVCount() > 1) {
        // multiple tex coord detected
        printf("Multiple diffuse tex coord detected\n");
    }
    // specular tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureSpecular) == 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureSpecular);
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->specularTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->specularTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->specularTexCoord->texCoords, mesh);
    } else if (mesh->GetElementUVCount() > 1) {
        // multiple tex coord detected
        printf("Multiple specular tex coord detected\n");
    }
    // ambient tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureAmbient) == 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureAmbient);
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->ambientTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->ambientTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->ambientTexCoord->texCoords, mesh);
    } else if (mesh->GetElementUVCount() > 1) {
        // multiple tex coord detected
        printf("Multiple ambient tex coord detected\n");
    }
    // normal tex coord
    if (mesh->GetElementUVCount(fbxsdk::FbxLayerElement::eTextureNormalMap) == 1) {
        FbxGeometryElementUV *etex = mesh->GetElementUV(0, fbxsdk::FbxLayerElement::eTextureNormalMap);
        if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->normalTexCoord->texCoordMapMode = AttributeMapMode_eByVertex;
        } else if (etex->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->normalTexCoord->texCoordMapMode = AttributeMapMode_eByIndex;
        } else {
            printf("unsupported tex coord mapping mode detected?\n");
        }
        ExtractAttributesToSceneFlatMesh(etex, ret->normalTexCoord->texCoords, mesh);
    } else if (mesh->GetElementUVCount() > 1) {
        // multiple tex coord detected
        printf("Multiple normal tex coord detected\n");
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
    return ret;
}

bool DumpMesh(FbxMesh *mesh, const MeshPrimitive *mp, FbxManager *fbxsdkMan) {
    if (mp->vertexs()) {
        mesh->InitControlPoints(mp->vertexs()->size());
        for (int i = 0; i < mp->vertexs()->size(); ++i) {
            auto vtx = mp->vertexs()->Get(i);
            FbxVector4 pt(vtx->x(), vtx->y(), vtx->z(), 1.0);
            mesh->SetControlPointAt(pt, i);
        }
    }
    if (mp->normals()) {
        if (mesh->GetUVLayerCount() == 0)
            mesh->CreateLayer();
        FbxLayerElementNormal *lLayerElementNormal = FbxLayerElementNormal::Create(mesh, "");
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
    return true;
}
