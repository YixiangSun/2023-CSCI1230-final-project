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
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
    std::vector<float> obj_vertexData;
};

class OBJparser
{
public:
    OBJparser();

    std::set<std::string> loadMesh(const char *filepath, std::unordered_map<std::string, OBJMaterial>& objects);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::unordered_map<std::string, OBJMaterial> parseMtlFile(const char *filepath); // std::string filename
};

#endif // OBJPARSER_H
