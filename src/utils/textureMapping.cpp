#include "utils/textureMapping.h"
#include "QtGui/qimage.h"
#include "utils/scenedata.h"
#include <iostream>
#include <ostream>

const float epsilon = 0.0001;

/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * @param file: file path to an image
 * @return a Texture struct with width, height, and pointer to the texture vector.
 */
Texture loadTextureFromFile(const QString &file) {
    QImage myTexture;
    int width; int height;
    Texture output;
    if (!myTexture.load(file)) {
        std::cout<<"Failed to load in image: " << file.toStdString() << std::endl;
        return output;
    }
    myTexture = myTexture.convertToFormat(QImage::Format_RGBX8888);
    width = myTexture.width();
    height = myTexture.height();

    RGBA* colorPointer = new RGBA[width*height];
    QByteArray arr = QByteArray::fromRawData((const char*) myTexture.bits(), myTexture.sizeInBytes());

    for (int i = 0; i < arr.size() / 4.f; i++){
        colorPointer[i] = RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]};
    }

    return Texture({width, height, colorPointer});
}



glm::vec4 findTextureColor(Texture texture, ScenePrimitive& primitive, glm::vec4 intersectPosObj) {
    if (texture.colorPointer == nullptr) return glm::vec4(0, 0, 0, 0);
    int m = primitive.material.textureMap.repeatU;
    int n = primitive.material.textureMap.repeatV;
    int w = texture.width;
    int h = texture.height;
    float u = 0;
    float v = 0;
    if (primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
        if (intersectPosObj[0] == 0.5) {
            u = -intersectPosObj[2] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
        else if (intersectPosObj[0] < -0.5+epsilon) {
            u = 0.5 + intersectPosObj[2];
            v = 0.5 + intersectPosObj[1];
        }
        else if (intersectPosObj[1] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = -intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[1] < -0.5+epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[2] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
        else if (intersectPosObj[2] < -0.5+epsilon) {
            u = -intersectPosObj[0] + 0.5;
            v = intersectPosObj[1] + 0.5;
        }
    }
    else if (primitive.type == PrimitiveType::PRIMITIVE_CYLINDER || primitive.type == PrimitiveType::PRIMITIVE_CONE) {
        if (intersectPosObj[1] > 0.5-epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = -intersectPosObj[2] + 0.5;
        }
        else if (intersectPosObj[1] < -0.5+epsilon) {
            u = intersectPosObj[0] + 0.5;
            v = intersectPosObj[2] + 0.5;
        } else {
            v = intersectPosObj[1] + 0.5;
            float theta = glm::atan(intersectPosObj[2], intersectPosObj[0]);
            theta < 0 ? u = -theta/(2.f*M_PI) : u = 1 - theta/(2.f*M_PI);
        }
    }
    else if (primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
        if (intersectPosObj[1] == -0.5 || intersectPosObj[1] == 0.5) u = 0.5;
        else {
            float theta = glm::atan(intersectPosObj[2], intersectPosObj[0]);
            theta < 0 ? u = -theta/(2.f*M_PI) : u = 1 - theta/(2.f*M_PI);
        }
        v = glm::asin(intersectPosObj[1]*2)/M_PI + 0.5f;
    }
    int c = int(std::floor(u * m * w)) % w;
    int r = int(std::floor((1-v) * n * h)) % h;
    RGBA color = texture.colorPointer[c + r * h];
    return glm::vec4(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
}
