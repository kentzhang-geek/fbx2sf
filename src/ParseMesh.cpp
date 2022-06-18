//
// Created by Admin on 2022/6/18.
//

#include "ParseMesh.h"

flatbuffers::Offset<MeshPrimitive> ParseMesh(FbxMesh *mesh, flatbuffers::FlatBufferBuilder &builder) {
    // vertex
    std::vector<Vec3d> points;
    for (int i = 0; i < mesh->GetControlPointsCount(); ++i) {
        auto pt = mesh->GetControlPointAt(i);
        points.push_back(Vec3d(pt[0], pt[1], pt[2]));
    }
    // indexes
    std::vector<TriangleIndexTuple> triangleIndexes;
    for (int i = 0; i < mesh->GetPolygonCount(); ++i) {
        if (mesh->GetPolygonSize(i) == 3) {
            triangleIndexes.push_back(TriangleIndexTuple(
                    flatbuffers::make_span({mesh->GetPolygonVertex(i, 0),
                                            mesh->GetPolygonVertex(i, 1),
                                            mesh->GetPolygonVertex(i, 2),
                                           })));
        } else {
            // encounter non triangle polygon, this is not good
            printf("Encounter Non Triangle Polygon!\n");
        }
    }
    return CreateMeshPrimitive(builder, builder.CreateVectorOfStructs(points),
                               builder.CreateVectorOfStructs(triangleIndexes));
}
