//
// Created by Admin on 2022/6/18.
//

#ifndef RPD_FBX_PLUGIN_INTERFACE_H
#define RPD_FBX_PLUGIN_INTERFACE_H

#include "macro.h"

EXTERN_C DLLEXPORT char * fbxFileToSceneBuffer(char * filename, int pathlen, int * size_len);
EXTERN_C DLLEXPORT bool sceneBufferToFbxFile(char * filename, int pathlen, char * buffer, int size_len);


#endif //RPD_FBX_PLUGIN_INTERFACE_H
