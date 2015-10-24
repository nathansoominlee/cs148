#include "assignment4/Utility.h"

#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Utility/Texture/TextureLoader.h"

// Returns an epic shader with the given parameters
// and texture. Any objects with different parameter/textures
// need different shaders.
std::shared_ptr<EpicShader> Utility::MakeEpicShader(float metallic, float roughness, float specular, std::string texture)
{
    std::unordered_map<GLenum, std::string> shaderSpec = { 
        { GL_VERTEX_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.vert" },
        { GL_FRAGMENT_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.frag"}
    };  

    std::shared_ptr<EpicShader> shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetMetallic(metallic);
    shader->SetRoughness(roughness);
    shader->SetSpecular(specular);

    if (!texture.empty())
    {   
        shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture(texture));
    }   

    return shader;
}


std::shared_ptr<BlinnPhongShader> Utility::MakeBlinnPhongShader(glm::vec4 diffuse, glm::vec4 specular, std::string texture)
{

    std::unordered_map<GLenum, std::string> shaderSpec = { 
        { GL_VERTEX_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.vert" },
        { GL_FRAGMENT_SHADER, "brdf/blinnphong/fragTexture/noSubroutine/blinnphong.frag"}
    };  

    std::shared_ptr<BlinnPhongShader> shader = std::make_shared<BlinnPhongShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetDiffuse(diffuse);
    float shininess = 40.f;
    shader->SetSpecular(specular, shininess); // TODO: take-in shininess from the spreadsheet

    if (!texture.empty())
    {
        shader->SetTexture(BlinnPhongShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture(texture));
        shader->SetTexture(BlinnPhongShader::TextureSlots::SPECULAR, TextureLoader::LoadTexture(texture));
    }

    return shader;
}


std::shared_ptr<class SceneObject> Utility::LoadObj(std::shared_ptr<class ShaderProgram> shader, std::shared_ptr<Scene> scene, std::string object_path)
{
    std::vector<std::shared_ptr<RenderingObject>> meshTemplate = MeshLoader::LoadMesh(shader, object_path);
    if (meshTemplate.empty()) 
    {   
        std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
        return nullptr;
    }           

    // Make scene object and add it to the scene
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>(meshTemplate);
    scene->AddSceneObject(sceneObject);

    // Return handle to scene object so caller can translate/rotate/scale it
    return sceneObject;
}
