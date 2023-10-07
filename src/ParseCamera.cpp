//
// Created by administrator on 10/6/2023.
//

#include "ParseCamera.h"

std::unique_ptr<CameraT> ParseCamera(FbxCamera *cam) {
    auto ret = std::make_unique<CameraT>();
    ret->name.assign(cam->GetName());
    auto eye = cam->EvaluatePosition();
    ret->eye = std::make_unique<Vec3d>(eye[0] / eye[3], eye[1] / eye[3], eye[2] / eye[3]);
    auto center = cam->EvaluateLookAtPosition();
    ret->center = std::make_unique<Vec3d>(center[0] / center[3], center[1] / center[3], center[2] / center[3]);
    auto up = cam->EvaluateUpDirection(eye, center);
    ret->upAxis = std::make_unique<Vec3d>(up[0] / up[3], up[1] / up[3], up[2] / up[3]);
    return ret;
}

// TODO dump camera, currently no dump