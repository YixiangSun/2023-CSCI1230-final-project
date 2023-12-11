#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <set>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Define structures to store material properties
struct OBJMaterial {
    std::string name;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
    std::vector<float> obj_vertexData;
};

class OBJparser
{
public:
    OBJparser();

    std::set<std::string> loadMesh(std::string filename, std::unordered_map<std::string, OBJMaterial>& objects);
//    void makeCorner(std::string corner, std::vector<float> vp,
//                    std::vector<float> vt, std::vector<float> vn,
//                    std::vector<float>& vertices);
    std::vector<std::string> split(std::string& str, char delimiter);
    std::unordered_map<std::string, OBJMaterial> parseMtlFile(std::string filename);
};

#endif // OBJPARSER_H
