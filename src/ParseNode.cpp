//
// Created by Admin on 2022/6/18.
//

#include "ParseNode.h"
#include "ParseMesh.h"
#include "tool.h"
#include "ParseCamera.h"
#include "ParseLight.h"


std::unique_ptr<BVHNodeT> ParseNode(FbxNode *node) {
    std::unique_ptr<BVHNodeT> ret(new BVHNodeT());
    ret->name.assign(node->GetName());
    std::vector<flatbuffers::Offset<MeshPrimitive>> meshes;
    // parse self
    for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {
        auto attri = node->GetNodeAttributeByIndex(i);
        switch (attri->GetAttributeType()) {
            case fbxsdk::FbxNodeAttribute::eMesh: {
                FbxMesh *mesh = FbxCast<FbxMesh>(attri);
                if (mesh) {
                    ret->meshes.push_back(ParseMesh(mesh));
                }
            } break;
            case fbxsdk::FbxNodeAttribute::eCamera: {
                FbxCamera * cam = FbxCast<FbxCamera>(attri);
                if (cam) {
                    SceneSingleton::get_mutable_instance().value->cameras.push_back(ParseCamera(cam));
                }
            } break;
            case fbxsdk::FbxNodeAttribute::eLight: {
                FbxLight * light = FbxCast<FbxLight>(attri);
                if (light) {
                    SceneSingleton::get_mutable_instance().value->lights.push_back(ParseLight(light, node));
                }
            }
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
            DumpMesh(newmesh, m, fbxsdkMan, fbxout);
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
