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

FbxVector4 DumpVec4d(Vec4d v) {
    return FbxVector4(v.x(), v.y(), v.z(), v.w());
}

FbxAMatrix DumpTransform(Matrix44d m) {
    FbxAMatrix ret;
    for (int i = 0; i < 4; ++i) {
        auto v = DumpVec4d(*m.idx()->Get(i));
        ret.SetRow(i, v);
    }
    return ret;
}

std::unique_ptr<BVHNodeT> ParseNode(FbxNode *node) {
    std::unique_ptr<BVHNodeT> ret(new BVHNodeT());
    ret->name.assign(node->GetName());
    std::vector<flatbuffers::Offset<MeshPrimitive>> meshes;
    // parse self
    for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {
        auto attri = node->GetNodeAttributeByIndex(i);
        if (attri->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) {
            // Attribute to mesh
            FbxMesh *mesh = FbxCast<FbxMesh>(attri);
            if (mesh) {
                ret->meshes.push_back(ParseMesh(mesh));
            }
        }
    }
    // parse material : TODO
    if (node->GetMaterialCount() > 0) {
        printf("material count: %d\n", node->GetMaterialCount());
        FbxSurfaceMaterial * mat = node->GetMaterial(0);
        printf("material name %s\n", mat->GetName());
        FbxSurfacePhong * phong = FbxCast<FbxSurfacePhong>(mat);
        printf("Phong mat unique id %d\n", phong->GetUniqueID());
        FbxFileTexture * tex = FbxCast<FbxFileTexture>(phong->Diffuse.GetSrcObject());
        if (tex) {
            printf("Texture name %s\n", tex->GetFileName());
        }
    }
    // translation
    auto m = FbxAMaxtirxToFlatbuffersMatrix(node->EvaluateLocalTransform());
    ret->local_transform.reset(new Matrix44d(m));
    m = FbxAMaxtirxToFlatbuffersMatrix(node->EvaluateGlobalTransform());
    ret->global_transform.reset(new Matrix44d(m));
    // parse children
    for (int i = 0; i < node->GetChildCount(); ++i) {
        ret->children.push_back(ParseNode(node->GetChild(i)));
    }
    return ret;
}

bool DumpNode(const BVHNode *node, FbxNode *fbxout, FbxManager *fbxsdkMan) {
    fbxout->SetName(node->name()->c_str());
    // local transform
    FbxAMatrix localM = DumpTransform(*node->local_transform());
    fbxout->LclTranslation = localM.GetT();
    fbxout->LclRotation = localM.GetR();
    fbxout->LclScaling = localM.GetS();
    // parse meshes
    if (node->meshes()) {
        for (int i = 0; i < node->meshes()->size(); ++i) {
            const MeshPrimitive *m = node->meshes()->Get(i);
            FbxMesh *newmesh = FbxMesh::Create(fbxsdkMan, "");
            DumpMesh(newmesh, m, fbxsdkMan);
            fbxout->AddNodeAttribute(newmesh);
        }
    }
    // parse children
    if (node->children()) {
        for (int i = 0; i < node->children()->size(); ++i) {
            const BVHNode *n = node->children()->Get(i);
            FbxNode *newfbxnode = FbxNode::Create(fbxsdkMan, n->name()->c_str());
            DumpNode(n, newfbxnode, fbxsdkMan);
            fbxout->AddChild(newfbxnode);
        }
    }
    return true;
}
