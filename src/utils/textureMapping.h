#ifndef TEXTUREMAPPING_H
#define TEXTUREMAPPING_H

#include "QtCore/qstring.h"
#include "utils/scenedata.h"
#include "utils/rgba.h"
#endif // TEXTUREMAPPING_H

glm::vec4 findTextureColor(Texture texture, ScenePrimitive& primitive, glm::vec4 intersectPosObj);
Texture loadTextureFromFile(const QString &file);
