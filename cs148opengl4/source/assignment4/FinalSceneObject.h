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
                      std::vector<FinalSceneObject>& final_scene_objects);  // output parameter

    // Print function for the created container
    static void PrintContainer(const std::vector<FinalSceneObject>& final_scene_objects);

    // Utility function to add a container of objects to the scene
    static void AddContainer(const std::vector<FinalSceneObject>& final_scene_objects, std::shared_ptr<Scene> scene);

private:

// The column data looks like this
// Try to parse? object description texture shader:p1,p2,... scale transformations:tx,ty,tz 

    enum Column {TryToParse      = 0,
                 Object          = 1,
                 Description     = 2,
                 Texture         = 3,
                 Shader          = 4,
                 Scale           = 5,
                 Transformations = 6,
                 Rotations       = 7
                };

    enum class ShaderType { None, Epic, BP };

    FinalSceneObject(std::string name, 
                     std::string description, 
                     std::string texture,
                     ShaderType shader_type,
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
        texture(texture),
        shader_type(shader_type),
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

    // Non-static private member functions
    std::shared_ptr<class ShaderProgram> MakeShader();
    std::shared_ptr<class SceneObject> LoadObj(std::shared_ptr<class ShaderProgram> shader, std::shared_ptr<Scene> scene);

    // Static private member functions
    static void PrintRow(std::vector<std::string> row);

    static FinalSceneObject ParseFSO(std::vector<std::string> row);

    static int ParseShader(const std::string& field,          // input parameter
                           ShaderType& shader_type,  // output parameter
                           float* epic_metallic,              // output parameter
                           float* epic_roughness,             // output parameter
                           float* epic_specular,              // output parameter
                           glm::vec4& bp_diffuse,                 // output parameter
                           glm::vec4& bp_specular);               // output parameter

    static int ParseEpicParams(const std::string& epic_params, // input parameter
                               float* epic_metallic,           // output parameter
                               float* epic_roughness,          // output parameter
                               float* epic_specular);          // output parameter

    static int ParseBPParams(const std::string& bp_params, // input parameter
                             glm::vec4 &bp_diffuse,        // output parameter
                             glm::vec4 &bp_specular);   // output parameter

    static int ParseTransformations(const std::string& t_params,
                                    float* tx, float* ty, float* tz);

    static int ParseRotations(const std::string& r_params,
                              float* rx, float* ry, float* rz);

    static int ParseScale(const std::string& scale_param,
                          float* scale);

    static std::size_t ReadFloat(const std::string& in_float, float* out_float);

    //  operator overload
    friend std::ostream& operator<< (std::ostream& os, const FinalSceneObject& fso);

    std::string name;
    std::string description;
    std::string texture;
    ShaderType shader_type;
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
