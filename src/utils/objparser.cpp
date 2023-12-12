#include "objparser.h"
#include <set>
#include <unordered_map>
#include <GL/glew.h>
#include <QFile>
#include <QTextStream>
#include <iostream>

OBJparser::OBJparser(){}

std::set<std::string> OBJparser::loadMesh(const char *filepath, std::unordered_map<std::string, OBJMaterial>& objects){ // std::unordered_map<std::string, Material>
    std::vector<float> vp;
    std::vector<float> vn;
//    std::vector<float> vertices;
    std::set<std::string> objNames;

    QString filepathStr = QString(filepath);
    QFile file(filepathStr);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString line;

        std::string currentName = "FALSE";
        std::vector<float> curr_vertexData;
        curr_vertexData.clear();
        while (!(line = stream.readLine()).isNull()) {
            std::vector<std::string> words = OBJparser::split(line.toStdString(), ' ');

            if (words[0] == "v") {
                vp.push_back(std::stof(words[1]));
                vp.push_back(std::stof(words[2]));
                vp.push_back(std::stof(words[3]));
            } else if (words[0] == "vn") {
                vn.push_back(std::stof(words[1]));
                vn.push_back(std::stof(words[2]));
                vn.push_back(std::stof(words[3]));
            } else if (words[0] == "f") { // !!!!!!!!!!!!!!!!

                std::vector <std::string> v_vt_vn1 = split(words[1], '/');
                std::vector <std::string> v_vt_vn2 = split(words[2], '/');
                std::vector <std::string> v_vt_vn3 = split(words[3], '/');
                // vertex positions and vertex normals
                int firstPos = (std::stoi(v_vt_vn1[0]) - 1) * 3;
                int firstNom = (std::stoi(v_vt_vn1[2]) - 1) * 3;
                curr_vertexData.push_back(vp[firstPos]);
                curr_vertexData.push_back(vp[firstPos + 1]);
                curr_vertexData.push_back(vp[firstPos + 2]);
                curr_vertexData.push_back(vn[firstNom]);
                curr_vertexData.push_back(vn[firstNom + 1]);
                curr_vertexData.push_back(vn[firstNom + 2]);

                int secondPos = (std::stoi(v_vt_vn2[0]) - 1) * 3;
                int secondNom = (std::stoi(v_vt_vn2[2]) - 1) * 3;
                curr_vertexData.push_back(vp[secondPos]);
                curr_vertexData.push_back(vp[secondPos + 1]);
                curr_vertexData.push_back(vp[secondPos + 2]);
                curr_vertexData.push_back(vn[secondNom]);
                curr_vertexData.push_back(vn[secondNom + 1]);
                curr_vertexData.push_back(vn[secondNom + 2]);

                int thirdPos = (std::stoi(v_vt_vn3[0]) - 1) * 3;
                int thirdNom = (std::stoi(v_vt_vn3[2]) - 1) * 3;
                curr_vertexData.push_back(vp[thirdPos]);
                curr_vertexData.push_back(vp[thirdPos + 1]);
                curr_vertexData.push_back(vp[thirdPos + 2]);
                curr_vertexData.push_back(vn[thirdNom]);
                curr_vertexData.push_back(vn[thirdNom + 1]);
                curr_vertexData.push_back(vn[thirdNom + 2]);
            } else if (words[0] == "usemtl") {
                if (currentName != "FALSE") {
                    objects[currentName].obj_vertexData.insert(objects[currentName].obj_vertexData.end(),
                                                               curr_vertexData.begin(),
                                                               curr_vertexData.end());
                    curr_vertexData.clear();
                }

                currentName = words[1];
                objNames.insert(currentName);
            }
        }

        if (currentName != "FALSE") { // parse the last element
            objects[currentName].obj_vertexData.insert(objects[currentName].obj_vertexData.end(),
                                                       curr_vertexData.begin(),
                                                       curr_vertexData.end());
        }

        file.close();
    } else {
        std::cout<<"Unable to open file" << std::endl;
    }
//    return vertices;
    return objNames;
}

std::vector<std::string> OBJparser::split(const std::string& str, char delimiter){
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;

    while(std::getline(iss, token, delimiter)){
        if (!token.empty()){
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Parse MTL file and store materials in a map
std::unordered_map<std::string, OBJMaterial> OBJparser::parseMtlFile(const char *filepath) { // std::string filename

    QString filepathStr = QString(filepath);
    QFile file(filepathStr);

    std::unordered_map<std::string, OBJMaterial> objects;
    objects.clear();

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        OBJMaterial currentMaterial;  // Track the current material being parsed
        std::string currentMaterialName = "False";

        QTextStream stream(&file);
        QString line;

        while (!(line = stream.readLine()).isNull()) {
            std::istringstream iss(line.toStdString());
            std::string token;
            iss >> token;

            if (token == "newmtl") {

                if (currentMaterialName != "False") {
                    objects[currentMaterialName] = currentMaterial;
                }
                // Initialize a new material
                currentMaterial = {};
                currentMaterial.obj_vertexData.clear();
                iss >> currentMaterial.name;
                currentMaterialName = currentMaterial.name;
            } else if (token == "Ka") {
                iss >> currentMaterial.ambient[0] >> currentMaterial.ambient[1] >> currentMaterial.ambient[2];
                currentMaterial.ambient[3] = 0;

//                std::cout << "Ambient Term: ";
//                for (int i = 0; i <= 3; ++i) {
//                    std::cout << currentMaterial.ambient[i] << " ";
//                }
//                std::cout << std::endl;

            } else if (token == "Kd") {
                iss >> currentMaterial.diffuse[0] >> currentMaterial.diffuse[1] >> currentMaterial.diffuse[2];
                currentMaterial.diffuse[3] = 0;

//                std::cout << "Diffuse Term: ";
//                for (int i = 0; i <= 3; ++i) {
//                    std::cout << currentMaterial.diffuse[i] << " ";
//                }
//                std::cout << std::endl;

            } else if (token == "Ks") {
                iss >> currentMaterial.specular[0] >> currentMaterial.specular[1] >> currentMaterial.specular[2];
                currentMaterial.specular[3] = 0; // ??????
            } else if (token == "Ns") {
                iss >> currentMaterial.shininess;
//                currentMaterial.shininess /= 255.f; // ??????
            }
            // Additional material properties can be handled similarly
            // Ke 0.000000 0.000000 0.000000 // ????
            // Ni 1.450000 // ????
            // d 1.000000 // ????
            // illum 2 // ????
        }

        if (currentMaterialName != "False") {
            objects[currentMaterialName] = currentMaterial;
        }

        file.close();
    } else {
        throw std::runtime_error(std::string("Failed to open file:") + filepath);
    }

    return objects;
}
