#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

glm::mat4 getTransformationMatrix(SceneNode* node) {
    glm::mat4 out = glm::mat4(1.0f);
    for (int i = 0; i < node->transformations.size(); i++) {
        SceneTransformation* t = node->transformations[i];
        switch(t->type){
        case TransformationType::TRANSFORMATION_TRANSLATE:
            out *= glm::translate(t->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            out *= glm::scale(t->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            out *= glm::rotate(t->angle, t->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            return t->matrix;
        default:
            break;
        }
    }
    return out;
}

void dfsTransform(std::vector<SceneLightData> &lights, std::vector<RenderShapeData> &shapes, glm::mat4 mat, SceneNode* root){
    mat = mat * getTransformationMatrix(root);
    if (root->children == std::vector<SceneNode*>()) {
        for (auto p : root->primitives) {
            RenderShapeData shape;
            shape.ctm = mat;
            shape.primitive = *p;
            shapes.push_back(shape);
        }
    }
    for (auto light : root->lights) {
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light -> color;
        lightData.function = light -> function;
        if (light->type != LightType::LIGHT_DIRECTIONAL) {
            lightData.pos = mat * glm::vec4(0.f, 0.f, 0.f, 1.f);
        }
        if (light->type != LightType::LIGHT_POINT) {
            lightData.dir = mat * light->dir;
        }
        if (light->type == LightType::LIGHT_SPOT) {
            lightData.angle = light->angle;
            lightData.penumbra = light->penumbra;
        }
        lights.push_back(lightData);
    }
    for (auto child : root->children) {
        dfsTransform(lights, shapes, mat, child);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();
    glm::mat4 ctm = glm::mat4(1.0f);
    dfsTransform(renderData.lights, renderData.shapes, ctm, root);

    return true;
}
