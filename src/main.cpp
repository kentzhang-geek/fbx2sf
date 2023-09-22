//
// Created by Admin on 2022/6/18.
//
#include "fileToScene.h"
#include "interface.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char **argv) {
    if (argc == 3) {
        int datasize;
        std::string path(argv[1]);
        char * buffer = fbxFileToSceneBuffer((char *)path.c_str(), path.size(), &datasize);
        std::string outfile(argv[2]);
        // write data to file
        std::ofstream ofile(outfile, std::ios::binary);
        ofile.write((char *)buffer, datasize);
        ofile.close();
        exit(0);
    } else if (argc == 4) {
        if (std::string(argv[1]) == "-r") {
            std::ifstream infile(argv[2], std::ios::binary);
            std::vector<char> buffer(std::istreambuf_iterator<char>(infile), {});
            std::string fpath = argv[3];
            sceneBufferToFbxFile((char *)fpath.c_str(), fpath.size(), buffer.data(), buffer.size());
            exit(0);
        }
    }

    // print usage
    {
        std::cout << "Usage: fbx2sf <input.fbx> <output.sf>" << std::endl;
        std::cout << "Usage: fbx2sf -r <input.sf> <output.fbx>" << std::endl;
        exit(0);
    }

    return 0;
}
