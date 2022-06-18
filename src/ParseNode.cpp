//
// Created by Admin on 2022/6/18.
//

#include "ParseNode.h"
#include "ParseMesh.h"

Matrix44d FbxAMaxtirxToFlatbuffersMatrix(FbxAMatrix am) {
    Matrix44d ret;
    Vec4d row0(am.Get(0, 0), am.Get(0, 1), am.Get(0, 2), am.Get(0, 3));
    Vec4d row1(am.Get(1, 0), am.Get(1, 1), am.Get(1, 2), am.Get(1, 3));
    Vec4d row2(am.Get(2, 0), am.Get(2, 1), am.Get(2, 2), am.Get(2, 3));
    Vec4d row3(am.Get(3, 0), am.Get(3, 1), am.Get(3, 2), am.Get(3, 3));
    return Matrix44d(flatbuffers::make_span({row0, row1, row2, row3}));
}

flatbuffers::Offset<BVHNode> ParseNode(FbxNode *node, flatbuffers::FlatBufferBuilder &builder) {
    BVHNodeBuilder bvhbuilder(builder);
    bvhbuilder.add_name(builder.CreateString(node->GetName()));
    std::vector<flatbuffers::Offset<MeshPrimitive>> meshes;
    // parse self
    for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {
        auto attri = node->GetNodeAttributeByIndex(i);
        if (attri->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) {
            // Attribute to mesh
            FbxMesh *mesh = FbxCast<FbxMesh>(attri);
            if (mesh) {
                meshes.push_back(ParseMesh(mesh, builder));
            }
        }
    }
    bvhbuilder.add_meshes(builder.CreateVector(meshes));
    // translation
    auto m = FbxAMaxtirxToFlatbuffersMatrix(node->EvaluateLocalTransform());
    bvhbuilder.add_local_transform(&m);
    m = FbxAMaxtirxToFlatbuffersMatrix(node->EvaluateGlobalTransform());
    bvhbuilder.add_global_transform(&m);
    // parse children
    std::vector<flatbuffers::Offset<BVHNode>> children;
    for (int i = 0; i < node->GetChildCount(); ++i) {
        auto child = ParseNode(node->GetChild(i), builder);
        children.push_back(child);
    }
    bvhbuilder.add_children(builder.CreateVector(children));
    return bvhbuilder.Finish();
}
