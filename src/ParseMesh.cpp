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
