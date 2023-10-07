//
// Created by administrator on 10/6/2023.
//

#ifndef RPD_FBX_PLUGIN_PARSECAMERA_H
#define RPD_FBX_PLUGIN_PARSECAMERA_H

#include "scene.h"
#include "bvh_accel.h"
#include "camera.h"
#include "fbxsdk.h"

std::unique_ptr<CameraT> ParseCamera(FbxCamera *cam);


#endif //RPD_FBX_PLUGIN_PARSECAMERA_H
