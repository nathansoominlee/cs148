#pragma once

#ifndef __FINALSCENEOBJECT__
#define __FINALSCENEOBJECT__

#include "common/core.h"
#include "assignment4/SheetReader.h"

class FinalSceneObject : public SceneObject
{

public:

    // Factory function to create a collection from the google sheet
    static int MakeContainer(const Rows &rows,                              // input parameter
                      std::vector<FinalSceneObject> &final_scene_objects);  // output parameter

private:

// The column data looks like this
// Try to parse? object description texture shader:p1,p2{,p3} transformations:tx,ty,tz 

    enum class Column {TryToParse      = 0,
                       Object          = 1,
                       Description     = 2,
                       Texture         = 3,
                       Shader          = 4,
                       Transformations = 5,
                       Rotations       = 6
                      };

    enum class Shader { Epic, BP };

    FinalSceneObject(std::string name, 
                     std::string description, 
                     std::string texture_path,
                     FinalSceneObject::Shader shader,
                     float epic_metallic,
                     float epic_roughness,
                     float epic_specular,
                     glm::vec4 bp_diffuse,
                     glm::vec4 bp_specular,
                     float scale,
                     float tx,
                     float ty,
                     float tz,
                     float rx,
                     float ry,
                     float rz) :
        name(name),
        description(description),
        texture_path(texture_path),
        shader(shader),
        epic_metallic(epic_metallic),
        epic_roughness(epic_roughness),
        epic_specular(epic_specular),
        bp_diffuse(bp_diffuse),
        bp_specular(bp_specular),
        scale(scale),
        tx(tx),
        ty(ty),
        tz(tz),
        rx(rx),
        ry(ry),
        rz(rz)
    {}


    static int ParseShader(const std::string& field,          // input parameter
                           FinalSceneObject::Shader& shader,  // output parameter
                           float& epic_metallic,              // output parameter
                           float& epic_roughness,             // output parameter
                           float& epic_specular,              // output parameter
                           float& bp_diffuse,                 // output parameter
                           float& bp_specular);               // output parameter

    static int ParseEpicParams(const std::string& epic_params, // input parameter
                               float& epic_metallic,           // output parameter
                               float& epic_roughness,          // output parameter
                               float& epic_specular);          // output parameter

    static int ParseBPParams(const std::string& bp_params, // input parameter
                             glm::vec4 &bp_diffuse,        // output parameter
                             glm::vec4 &epic_roughness);   // output parameter


    std::string name;
    std::string description;
    std::string texture_path;
    FinalSceneObject::Shader shader;
    float epic_metallic;
    float epic_roughness;
    float epic_specular;
    glm::vec4 bp_diffuse;
    glm::vec4 bp_specular;
    float scale;
    float tx;
    float ty;
    float tz;
    float rx;
    float ry;
    float rz;

};

#endif
