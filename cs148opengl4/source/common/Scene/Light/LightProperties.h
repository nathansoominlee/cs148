#pragma once

#ifndef __LIGHT_PROPERTIES__
#define __LIGHT_PROPERTIES__

#include "common/common.h"

struct LightProperties {

    // BlinnPhong members
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    // EpicShader members
    glm::vec4 color;
    glm::vec4 direction;
    glm::vec3 groundColor;  // for hemispheric light. in rgb.
    glm::vec3 skyColor;     // for hemispheric light. in rgb.
    float radius;

};

#endif
