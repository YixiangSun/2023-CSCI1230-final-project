#include "fire.h"
#include "utils/scenedata.h"

ScenePrimitive smokePrimitive{
    .type = PrimitiveType::PRIMITIVE_CUBE,
    .material = smoke,
    .isFire = false,
    .isSmoke = true,
};

fire::fire()
{

}
