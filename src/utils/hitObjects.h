#ifndef HITOBJECTS_H
#define HITOBJECTS_H

#include <vector>
#endif // HITOBJECTS_H

enum class HitObjType {
    HitObj_Sphere,
    HitObj_Cylinder,
};

struct hitObject {
    float position[2];
    float radius;
    HitObjType type;
};
