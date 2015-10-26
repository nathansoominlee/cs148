#pragma once

#ifndef __UTILITY__
#define __UTILITY__

#include "common/core.h"

class Utility
{

public:

    static void AddPLight(glm::vec4 color, glm::vec3 position, std::shared_ptr<Scene> scene);

    static std::shared_ptr<EpicShader> MakeEpicShader(float metallic, float roughness, float specular, std::string texture);

    static std::shared_ptr<BlinnPhongShader> MakeBlinnPhongShader(glm::vec4 diffuse, glm::vec4 specular, std::string texture);

    static std::shared_ptr<class SceneObject> LoadObj(std::shared_ptr<class ShaderProgram> shader, std::shared_ptr<Scene> scene, std::string object_path);

};

#endif
