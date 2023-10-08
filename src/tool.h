//
// Created by administrator on 9/30/2023.
//

#ifndef RPD_FBX_PLUGIN_TOOL_H
#define RPD_FBX_PLUGIN_TOOL_H

#include <memory>
#include <atomic>
#include <fstream>
#include "common.h"
#include <iostream>
#include <utility>
#include <unordered_map>
#include <boost/serialization/config.hpp>
#include <boost/serialization/singleton.hpp>
#include "fbxsdk.h"

using namespace sf;

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

static inline std::string to_string(FbxVector4 v) {
    return std::string("(") + std::to_string(v[0]) + " " + std::to_string(v[1]) + " " + std::to_string(v[2]) + " " + std::to_string(v[3]) + ")";
}

typedef boost::serialization::singleton<std::unordered_map<uint64_t, FbxSurfaceMaterial*>> MaterialMapUniqueID;

template <typename T> class SingleContainer {
public:
    T value;
    SingleContainer(const T &input) {
        value = input;
    }

    SingleContainer() = default;

    T operator = (const T &input) {
        value = input;
        return value;
    }
};

static inline Matrix44d FbxAMaxtirxToFlatbuffersMatrix(FbxAMatrix am) {
    Matrix44d ret;
    Vec4d row0(am.Get(0, 0), am.Get(0, 1), am.Get(0, 2), am.Get(0, 3));
    Vec4d row1(am.Get(1, 0), am.Get(1, 1), am.Get(1, 2), am.Get(1, 3));
    Vec4d row2(am.Get(2, 0), am.Get(2, 1), am.Get(2, 2), am.Get(2, 3));
    Vec4d row3(am.Get(3, 0), am.Get(3, 1), am.Get(3, 2), am.Get(3, 3));
    return Matrix44d(flatbuffers::make_span({row0, row1, row2, row3}));
}

static inline FbxVector4 DumpVec4d(Vec4d v) {
    return FbxVector4(v.x(), v.y(), v.z(), v.w());
}

static inline FbxAMatrix DumpTransform(Matrix44d m) {
    FbxAMatrix ret;
    for (int i = 0; i < 4; ++i) {
        auto v = DumpVec4d(*m.rows()->Get(i));
        ret.SetRow(i, v);
    }
    return ret;
}

namespace sf {
    struct SceneT;
}
typedef boost::serialization::singleton<SingleContainer<sf::SceneT *>> SceneSingleton;

//#define DEBUG_MODE

#endif //RPD_FBX_PLUGIN_TOOL_H
