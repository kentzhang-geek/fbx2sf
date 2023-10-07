//
// Created by administrator on 10/6/2023.
//

#include "ParseLight.h"
#include "tool.h"

std::unique_ptr<LightT> ParseLight(FbxLight *light, FbxNode *node) {
    std::unique_ptr<LightT> ret = std::make_unique<LightT>();
    auto m = FbxAMaxtirxToFlatbuffersMatrix(node->EvaluateGlobalTransform());
    ret->global_transform.reset(new Matrix44d(m));
    ret->name = light->GetName();

    LightType map[] = {LightType_ePoint, LightType_eDirection, LightType_eSpot, LightType_eArea, LightType_eNone};
    ret->type = map[light->LightType.Get()];

    auto color = light->Color.Get();
    ret->color = std::make_unique<Vec3d>(color[0], color[1], color[2]);
    ret->intensity = light->Intensity.Get();

    ret->innerAngle = light->InnerAngle.Get();
    ret->outerAngle = light->OuterAngle.Get();

    return ret;
}
