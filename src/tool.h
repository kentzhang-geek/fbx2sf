//
// Created by administrator on 9/30/2023.
//

#ifndef RPD_FBX_PLUGIN_TOOL_H
#define RPD_FBX_PLUGIN_TOOL_H

#include <memory>
#include <fstream>
#include "common.h"
#include <iostream>
#include <utility>
#include <unordered_map>
#include <boost/serialization/config.hpp>
#include <boost/serialization/singleton.hpp>
#include "fbxsdk.h"

#define RTP_LOG(...) internel_cpp_printer(__VA_ARGS__)

static inline void internel_cpp_printer() {
    std::cout << std::endl;
}

template <typename First, typename ...Args>
static inline void internel_cpp_printer(First&& first, Args&&... args)
{
    std::cout << first << " ";
    internel_cpp_printer(std::forward<Args>(args)...);
}

static inline std::unique_ptr<Vec4d> MakeVec4d(double x, double y, double z, double w) {
    return std::make_unique<Vec4d>(x,y,z, w);
};

static inline std::unique_ptr<Vec3d> MakeVec3d(double x, double y, double z) {
    return std::make_unique<Vec3d>(x,y,z);
};

static inline std::unique_ptr<Vec2d> MakeVec2d(double x, double y) {
    return std::make_unique<Vec2d>(x,y);
};

static inline std::vector<uint8_t> && ReadFileContent(std::string file_name) {
    // read file content to a byte vector
    std::ifstream ifs(file_name, std::ios::binary);
    if (!ifs.is_open())
    {
        RTP_LOG("Failed to open file:", file_name);
        return {};
    }
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(ifs), {});
    return std::forward<std::vector<uint8_t>>(buffer);
}

typedef boost::serialization::singleton<std::unordered_map<uint64_t, FbxSurfaceMaterial*>> MaterialMapUniqueID;

#endif //RPD_FBX_PLUGIN_TOOL_H
