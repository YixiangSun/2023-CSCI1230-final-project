#include "objparser.h"
#include <set>
#include <unordered_map>

//#include <filesystem> // ??????????????????????????????????

#include <GL/glew.h>
#include <QFile>
#include <QTextStream>
#include <iostream>

OBJparser::OBJparser(){}

//std::vector<float>
std::set<std::string> OBJparser::loadMesh(const char *filepath, std::unordered_map<std::string, OBJMaterial>& objects){ // std::unordered_map<std::string, Material>
    std::vector<float> vp;
//    std::vector<float> vt;
    std::vector<float> vn;
//    std::vector<float> vertices;
    std::set<std::string> objNames;

//    std::ifstream file;
//    file.open(filename);
    QString filepathStr = QString(filepath);
    QFile file(filepathStr);

//    if(file.is_open()){
//        std::string line;

//        OBJMaterial currentOBJ; // ??????????????????????
//        currentOBJ.obj_vertexData.clear();
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString line;

        std::string currentName = "FALSE";
        OBJMaterial currentOBJ;
        currentOBJ.obj_vertexData.clear();
//        while(getline(file, line)){
        while (!(line = stream.readLine()).isNull()) {
//            std::vector<std::string> words = OBJparser::split(line, ' ');
            std::vector<std::string> words = OBJparser::split(line.toStdString(), ' ');
//            std::istringstream iss(line.toStdString());
//            std::string token;
//            iss >> token;
            if (words[0] == "o") { // !!!!!!!!!!!!!!!!!!

                vp.clear();
//                vt.clear();
                vn.clear();
//                vertices.clear();

            } else if (words[0] == "v") {
                vp.push_back(std::stof(words[1]));
                vp.push_back(std::stof(words[2]));
                vp.push_back(std::stof(words[3]));
            } /*else if (words[0] == "vt") {
                vt.push_back(std::stof(words[1]));
                vt.push_back(std::stof(words[2]));
            }*/ else if (words[0] == "vn") {
                vn.push_back(std::stof(words[1]));
                vn.push_back(std::stof(words[2]));
                vn.push_back(std::stof(words[3]));
            } else if (words[0] == "f") { // !!!!!!!!!!!!!!!!
//                int triangleCount = words.size() - 3;
//                for (int i = 0; i < triangleCount; i ++){
////                    makeCorner(words[1], vp, vt, vn, vertices);
////                    makeCorner(words[2 + i], vp, vt, vn, vertices);
////                    makeCorner(words[3 + i], vp, vt, vn, vertices);
//                    makeCorner(words[1], vp, vt, vn, currentOBJ.obj_vertexData);
//                    makeCorner(words[2 + i], vp, vt, vn, currentOBJ.obj_vertexData);
//                    makeCorner(words[3 + i], vp, vt, vn, currentOBJ.obj_vertexData);
//                }
                int triangleCount = words.size() - 3;
                for (int i = 0; i < triangleCount; i ++){
                    // ??????????????????????????????????????????//
                    std::vector <std::string> v_vt_vn1 = split(words[1], '/');
                    std::vector <std::string> v_vt_vn2 = split(words[2 + i], '/');
                    std::vector <std::string> v_vt_vn3 = split(words[3 + i], '/');
                    // vertex positions
                    currentOBJ.obj_vertexData.push_back(vp[std::stoi(v_vt_vn1[0]) - 1]);
                    currentOBJ.obj_vertexData.push_back(vp[std::stoi(v_vt_vn2[0]) - 1]);
                    currentOBJ.obj_vertexData.push_back(vp[std::stoi(v_vt_vn3[0]) - 1]);
                    // vertex normals
                    currentOBJ.obj_vertexData.push_back(vn[std::stoi(v_vt_vn1[2]) - 1]);
                    currentOBJ.obj_vertexData.push_back(vn[std::stoi(v_vt_vn2[2]) - 1]);
                    currentOBJ.obj_vertexData.push_back(vn[std::stoi(v_vt_vn3[2]) - 1]);
                }
            } else if (words[0] == "usemtl") {
                if (currentName != "FALSE") { // ??????
                    objects[currentName].obj_vertexData.insert(objects[currentName].obj_vertexData.end(),
                                                               currentOBJ.obj_vertexData.begin(),
                                                               currentOBJ.obj_vertexData.end());
                }

                currentName = words[1]; // ????????????
                currentOBJ = objects[currentName];
                objNames.insert(currentName); // ????????????
            }
        }
        file.close();
    } else {
        std::cout<<"Unable to open file" << std::endl;
    }
//    return vertices;
    return objNames;
}

//void OBJparser::makeCorner(std::string corner, std::vector<float> v,
//                           std::vector<float> vt, std::vector<float> vn,
//                           std::vector<float>& vertices){ // !!!!!!!!!!
//    std::vector<std::string> v_vt_vn = split(corner, '/');
//    vertices.push_back(v[std::stoi(v_vt_vn[0]) - 1]);
//}

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
//    std::ifstream fileStream(filePath); // Open the file for reading

//     std::ifstream file(filename);
    QString filepathStr = QString(filepath);
    QFile file(filepathStr);

//    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream stream(&file);
//        code = stream.readAll().toStdString();
//    } else {
//        throw std::runtime_error(std::string("Failed to open shader:")+filepath);
//    }

//    // Compile shader code.
//    const char *codePtr = code.c_str();
//    if (!file.is_open()) {
//        std::cerr << "Error opening file: " << filename << std::endl;
//        return {};
//    }
    std::unordered_map<std::string, OBJMaterial> objects;
    objects.clear();
    OBJMaterial currentMaterial;  // Track the current material being parsed
    std::string currentMaterialName;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString line;
        while (!(line = stream.readLine()).isNull()) {
            std::istringstream iss(line.toStdString());
            std::string token;
            iss >> token;

            if (token == "newmtl") {
                // Initialize a new material
                currentMaterial = {};
                currentMaterial.obj_vertexData.clear();
                iss >> currentMaterial.name;
                currentMaterialName = currentMaterial.name;
            } else if (token == "Ka") {
                iss >> currentMaterial.ambient[0] >> currentMaterial.ambient[1] >> currentMaterial.ambient[2];
                currentMaterial.ambient[3] = 0; // ??????
            } else if (token == "Kd") {
                iss >> currentMaterial.diffuse[0] >> currentMaterial.diffuse[1] >> currentMaterial.diffuse[2];
                currentMaterial.diffuse[3] = 0; // ??????
            } else if (token == "Ks") {
                iss >> currentMaterial.specular[0] >> currentMaterial.specular[1] >> currentMaterial.specular[2];
                currentMaterial.specular[3] = 0; // ??????
            } else if (token == "Ns") {
                iss >> currentMaterial.shininess;
                currentMaterial.shininess /= 255.f; // ??????
            }
            // Additional material properties can be handled similarly
            // Ke 0.000000 0.000000 0.000000 // ????
            // Ni 1.450000 // ????
            // d 1.000000 // ????
            // illum 2 // ????
        }
        file.close();
    } else {
        throw std::runtime_error(std::string("Failed to open file:") + filepath);
    }

    return objects;
}
//    std::string line;
//    while (std::getline(file, line)) {
//        std::istringstream iss(line);
//        std::string token;
//        iss >> token;

//        if (token == "newmtl") {
////            // Start parsing a new material
////            if (!currentMaterialName.empty()) { // ????????
////                // Store the previously parsed material
////                objects[currentMaterialName] = currentMaterial;
////            }

//            // Initialize a new material
//            currentMaterial = {};
//            currentMaterial.obj_vertexData.clear(); // ???????
//            iss >> currentMaterial.name;
//            currentMaterialName = currentMaterial.name;
//        } else if (token == "Ka") {
//            iss >> currentMaterial.ambient[0] >> currentMaterial.ambient[1] >> currentMaterial.ambient[2];
//        } else if (token == "Kd") {
//            iss >> currentMaterial.diffuse[0] >> currentMaterial.diffuse[1] >> currentMaterial.diffuse[2];
//        } else if (token == "Ks") {
//            iss >> currentMaterial.specular[0] >> currentMaterial.specular[1] >> currentMaterial.specular[2];
//        } else if (token == "Ns") {
//            iss >> currentMaterial.shininess;
//            currentMaterial.shininess /= 255.f; // ???
//        }
//        // Additional material properties can be handled similarly
//        // Ke 0.000000 0.000000 0.000000 // ????
//        // Ni 1.450000 // ????
//        // d 1.000000 // ????
//        // illum 2 // ????

//    }

////    // Store the last parsed material
////    if (!currentMaterialName.empty()) {
////        objects[currentMaterialName] = currentMaterial;
////    }

//    file.close();
//    return objects;
