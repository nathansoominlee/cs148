#pragma once

#ifndef __EPIC_LIGHT_PROPERTIES__
#define __EPIC_LIGHT_PROPERTIES__

#include "common/Scene/Light/LightProperties.h"

struct EpicLightProperties: public LightProperties {
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
};

#endif
