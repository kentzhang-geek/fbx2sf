//
// Created by Admin on 2022/6/20.
//

#include "sceneToFile.h"
#include "fbxsdk.h"
#include "ParseNode.h"
#include "PraseMaterial.h"
#include "tool.h"

bool sceneToFile(std::string filename, Scene *scene) {
    FbxManager *lSdkManager = FbxManager::Create();

    // Create the IO settings object.
    FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
    lSdkManager->SetIOSettings(ios);

    // Create an exporter.
    FbxExporter *lExporter = FbxExporter::Create(lSdkManager, "");

    // Initialize the exporter.
    bool lExportStatus = lExporter->Initialize(filename.c_str(), -1, lSdkManager->GetIOSettings());
    if (!lExportStatus) {
        printf("Call to FbxExporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    // Create a new scene so that it can be populated by the imported file.
    FbxScene *lScene = FbxScene::Create(lSdkManager, "myScene");

    // dump material
    for (const Material * m : *scene->materials()) {
        FbxSurfaceMaterial *mat = DumpMaterial(m, lScene);
        lScene->AddMaterial(mat);
        MaterialMapUniqueID::get_mutable_instance()[m->id()] = mat;
    }

    // convert to scene
    DumpNode(scene->root(), lScene->GetRootNode(), lSdkManager);

#ifdef DEBUG_MODE
    FbxVector4 min, max, center;
    lScene->GetRootNode()->EvaluateGlobalBoundingBoxMinMaxCenter(min, max, center);
    RTP_LOG("Got bound box", to_string(min), to_string(max), to_string(center));
#endif

    // Export the scene to the file.
    lExporter->Export(lScene);

    lScene->Destroy();

    // The file is imported, so get rid of the importer.
    lExporter->Destroy();
}

