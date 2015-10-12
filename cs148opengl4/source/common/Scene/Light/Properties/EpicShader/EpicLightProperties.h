#pragma once

#ifndef __EPIC_LIGHT_PROPERTIES__
#define __EPIC_LIGHT_PROPERTIES__

#include "common/Scene/Light/LightProperties.h"

struct EpicLightProperties: public LightProperties {
    glm::vec4 color;
    glm::vec4 direction;
    glm::vec3 groundColor;  // for hemispheric light. in rgb.
    glm::vec3 skyColor;        // for hemispheric light. in rgb.
    float radius;
};

#endif
