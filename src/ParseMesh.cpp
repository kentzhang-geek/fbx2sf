//
// Created by Admin on 2022/6/18.
//

#include "ParseMesh.h"

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
    // normal
    if (mesh->GetElementNormalCount() == 1) {
        FbxGeometryElementNormal *enor = mesh->GetElementNormal(0);
        if (enor->GetMappingMode() == fbxsdk::FbxLayerElement::eByControlPoint) {
            ret->normalMapMode = NormalMapMode_eByVertex;
            for (int i = 0; i < mesh->GetControlPointsCount(); ++i) {
                if (enor->GetReferenceMode() == fbxsdk::FbxLayerElement::eDirect) {
                    auto nor = enor->GetDirectArray().GetAt(i);
                    ret->normals.push_back(Vec3d(nor[0], nor[1], nor[2]));
                } else if (enor->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect) {
                    auto nor = enor->GetDirectArray().GetAt(enor->GetIndexArray().GetAt(i));
                    ret->normals.push_back(Vec3d(nor[0], nor[1], nor[2]));
                } else {
                    printf("unsupported normal reference mode detected\n");
                }
            }
        } else if (enor->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex) {
            ret->normalMapMode = NormalMapMode_eByIndex;
            for (int i = 0; i < (mesh->GetPolygonCount() * 3); ++i) {
                if (enor->GetReferenceMode() == fbxsdk::FbxLayerElement::eDirect) {
                    auto nor = enor->GetDirectArray().GetAt(i);
                    ret->normals.push_back(Vec3d(nor[0], nor[1], nor[2]));
                } else if (enor->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect) {
                    auto nor = enor->GetDirectArray().GetAt(enor->GetIndexArray().GetAt(i));
                    ret->normals.push_back(Vec3d(nor[0], nor[1], nor[2]));
                } else {
                    printf("unsupported normal reference mode detected\n");
                }
            }
        } else {
            printf("unsupported normal mapping mode detected?\n");
        }
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
            case NormalMapMode::NormalMapMode_eByIndex : {
                lLayerElementNormal->SetMappingMode(fbxsdk::FbxLayerElement::eByPolygonVertex);
                break;
            }
            case NormalMapMode::NormalMapMode_eByVertex : {
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
