#include "common/Rendering/Shaders/EpicShader.h"
#include "common/Rendering/Textures/Texture.h"
#include "common/Scene/Light/Light.h"
#include "common/Scene/Light/Properties/EpicShader/EpicLightProperties.h"
#include "common/Scene/Camera/Camera.h"
#include "common/Utility/Texture/TextureLoader.h"

std::array<const char*, 3> EpicShader::MATERIAL_PROPERTY_NAMES = {
    "InputMaterial.metallic", 
    "InputMaterial.roughness", 
    "InputMaterial.specular"
};
const int EpicShader::MATERIAL_BINDING_POINT = 0;

EpicShader::EpicShader(const std::unordered_map<GLenum, std::string>& inputShaders, GLenum lightingStage):
    ShaderProgram(inputShaders), metallic(1.f), roughness(1.f), specular(1.f),
    materialBlockLocation(0), materialBlockSize(0), materialBuffer(0),
    lightingShaderStage(lightingStage)
{
    if (!shaderProgram) {
        return;
    }


    SetupUniformBlock<3>("InputMaterial", MATERIAL_PROPERTY_NAMES, materialIndices, materialOffsets, materialStorage, materialBlockLocation, materialBlockSize, materialBuffer);
    UpdateMaterialBlock();
    

#ifdef DISABLE_OPENGL_SUBROUTINES
    (void)lightingShaderStage;
#endif

    defaultTexture = TextureLoader::LoadTexture("required/defaultTexture.png");
    if (!defaultTexture) {
        std::cerr << "WARNING: Failed to load the default texture." << std::endl;
        return;
    }
}

EpicShader::~EpicShader()
{
    OGL_CALL(glDeleteBuffers(1, &materialBuffer));
}

void EpicShader::SetupShaderLighting(const Light* light) const
{
    if (!light) {
        SetShaderUniform("lightingType", static_cast<int>(Light::LightType::POINT));
    } else {
        // Select proper lighting subroutine based on the light's type.
        switch(light->GetLightType()) {
        case Light::LightType::POINT:
            SetShaderUniform("lightingType", static_cast<int>(Light::LightType::POINT));
            break;
        case Light::LightType::DIRECTIONAL:
            SetShaderUniform("lightingType", static_cast<int>(Light::LightType::DIRECTIONAL));
            break;
        case Light::LightType::HEMISPHERE:
            SetShaderUniform("lightingType", static_cast<int>(Light::LightType::HEMISPHERE));
            break;
        default:
            std::cerr << "WARNING: Light type is not supported. Defaulting to point light. Your output may look wrong. -- Ignoring: " << 
                static_cast<int>(light->GetLightType()) << std::endl;
            SetShaderUniform("lightingType", static_cast<int>(Light::LightType::POINT));
            break;
        }


        // Get the light's properties and pass it into the shader.
        const EpicLightProperties* lightProperty = static_cast<const EpicLightProperties*>(light->GetPropertiesRaw());
        SetShaderUniform("genericLight.color", lightProperty->color);
        SetShaderUniform("genericLight.direction", lightProperty->direction);
        SetShaderUniform("genericLight.groundColor", lightProperty->groundColor);
        SetShaderUniform("genericLight.skyColor", lightProperty->skyColor);
        SetShaderUniform("genericLight.radius", lightProperty->radius);
        light->SetupShaderUniforms(this);
    }
    UpdateAttenuationUniforms(light);
}

void EpicShader::UpdateMaterialBlock() const
{
    StartUseShader();

    memcpy((void*)(materialStorage.data() + materialOffsets[0]), &metallic, sizeof(float));
    memcpy((void*)(materialStorage.data() + materialOffsets[1]), &roughness, sizeof(float));
    memcpy((void*)(materialStorage.data() + materialOffsets[2]), &specular, sizeof(float));
    

    if (materialBuffer && materialBlockLocation != GL_INVALID_INDEX) {
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, materialBuffer));
        OGL_CALL(glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, materialStorage.data(), GL_STATIC_DRAW));
        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_POINT, materialBuffer));
        OGL_CALL(glUniformBlockBinding(shaderProgram, materialBlockLocation, MATERIAL_BINDING_POINT));
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    // While we're here, also setup the textures too.
    SetShaderUniform("diffuseTexture", static_cast<int>(TextureSlots::DIFFUSE));
    SetShaderUniform("specularTexture", static_cast<int>(TextureSlots::SPECULAR));

    StopUseShader();
}

void EpicShader::UpdateAttenuationUniforms(const Light* light) const
{
    float constant = 1.f, linear = 0.f, quadratic = 0.f;
    if (light) {
        light->GetAttenuation(constant, linear, quadratic);
    }

    SetShaderUniform("constantAttenuation", constant);
    SetShaderUniform("linearAttenuation", linear);
    SetShaderUniform("quadraticAttenuation", quadratic);
}

void EpicShader::SetupShaderMaterials() const
{
    // Need to make sure the material buffer is bound.
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_POINT, materialBuffer));

    // Make sure the right textures are bound.
    const Texture* diffuseTexture = defaultTexture.get();
    if (textureSlotMapping.find(TextureSlots::DIFFUSE) != textureSlotMapping.end()) {
        diffuseTexture = textureSlotMapping.at(TextureSlots::DIFFUSE).get();
    }
    assert(diffuseTexture);
    diffuseTexture->BeginRender(static_cast<int>(TextureSlots::DIFFUSE));

    const Texture* specularTexture = defaultTexture.get();
    if (textureSlotMapping.find(TextureSlots::SPECULAR) != textureSlotMapping.end()) {
        specularTexture = textureSlotMapping.at(TextureSlots::SPECULAR).get();
    }
    assert(specularTexture);
    specularTexture->BeginRender(static_cast<int>(TextureSlots::SPECULAR));

}

void EpicShader::SetupShaderCamera(const class Camera* camera) const
{
    SetShaderUniform("cameraPosition", camera->GetPosition());
}

std::unique_ptr<EpicLightProperties> EpicShader::CreateLightProperties()
{
    return make_unique<EpicLightProperties>();
}

void EpicShader::SetMetallic(float inMetallic)
{
    metallic = inMetallic;
    UpdateMaterialBlock();
}

void EpicShader::SetRoughness(float inRoughness)
{
    roughness = inRoughness;
    UpdateMaterialBlock();
}

void EpicShader::SetSpecular(float inSpecular)
{
    specular = inSpecular;
    UpdateMaterialBlock();
}

void EpicShader::SetTexture(TextureSlots::Type slot, std::shared_ptr<class Texture> inputTexture)
{
    textureSlotMapping[slot] = std::move(inputTexture);
}
