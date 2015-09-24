#pragma once

#ifndef __BLINN_PHONG_LIGHT_PROPERTIES__
#define __BLINN_PHONG_LIGHT_PROPERTIES__

#include "common/Scene/Light/LightProperties.h"

struct BlinnPhongLightProperties: public LightProperties {
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
};

#endif
