#pragma once

#ifndef __EPIC_SHADER__
#define __EPIC_SHADER__

#include "common/Rendering/Shaders/ShaderProgram.h"
#include <functional>

/*! \brief The CPU Interface to the Epic shader (either or: vert or frag version or the textured version).
 *
 *  Note that this shader class can also probably be used for any minor variations on the Epic shader.
 */
class EpicShader: public ShaderProgram
{
public:
    /*! \brief Construcs the Epic shader object.
     *  \param inputShaders Look at ShaderProgram::ShaderProgram() for details.
     *  \param lightingStage If subroutines are not disabled, this is the shader object (vertex or fragment) that contains the subroutine uniform variable.
     */
    EpicShader(const std::unordered_map<GLenum, std::string>& inputShaders, GLenum lightingStage);

    /*! \brief Deconstructor.
     */
    virtual ~EpicShader();

    /*! \copydoc ShaderProgram::SetupShaderLighting(const class Light* light) const;
     *  
     *  Makes sure the proper lighting function is called (i.e. for point light, directional, etc.). Additionally, passes in the light properties to the
     *  shader as well as any light attenuation properties.
     */
    virtual void SetupShaderLighting(const class Light* light) const; 

    /*! \copydoc ShaderProgram::SetupShaderMaterials() const
     *
     *  Makes sure that the uniform block object is bound and handles binding the diffuse and specular textures if they exist.
     */
    virtual void SetupShaderMaterials() const;

    /*! \copydoc ShaderProgram::SetupShaderCamera(const class Camera*) const
     *
     *  Copies the camera position to the shader.
     */
    virtual void SetupShaderCamera(const class Camera* camera) const;

    virtual void SetMetallic(float inMetallic);
    virtual void SetRoughness(float inRoughness);
    virtual void SetSpecular(float inSpecular);

    /*! \brief Corresponds to the texture unit that we want to bind the texture to.
     *
     *  We specify the active texture unit using <a href="https://www.opengl.org/sdk/docs/man/html/glActiveTexture.xhtml">glActiveTexture</a>. The texture unit 
     *  has a different role than the texture target (i.e. GL_TEXTURE_2D). You can read more about it <a href="https://www.opengl.org/wiki/Texture">here</a>.
     */
    struct TextureSlots {
        enum Type {
            DIFFUSE = 0,
            SPECULAR
        };
    };

    /*! \brief Stores the texture internally. Does not copy to OpenGL. 
     *  \param slot The texture unit that we will be binding the texture to.
     *  \param inputTexture A pointer to the assignment framework's representation of a texture.
     */
    virtual void SetTexture(TextureSlots::Type slot, std::shared_ptr<class Texture> inputTexture);

    /*! \copydoc ShaderProgram::CreateLightProperties() 
     */
    static std::unique_ptr<struct EpicLightProperties> CreateLightProperties();
protected:
    // Material Parameters
    virtual void UpdateMaterialBlock() const;
    float metallic;
    float roughness;
    float specular;
    /*
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
    glm::vec4 ambient;
    */

    // Material Bindings into the Shader
    static std::array<const char*, 3> MATERIAL_PROPERTY_NAMES;
    static const int MATERIAL_BINDING_POINT;
    GLuint materialBlockLocation;
    GLint materialBlockSize;
    std::array<GLuint, 3> materialIndices;
    std::array<GLint, 3> materialOffsets;
    GLuint materialBuffer;
    std::vector<GLubyte> materialStorage;

    // Attenuation Uniform Handling
    virtual void UpdateAttenuationUniforms(const class Light* light) const;

private:
    // Textures
    std::shared_ptr<class Texture> defaultTexture;
    std::unordered_map<TextureSlots::Type, std::shared_ptr<class Texture>, std::hash<int> > textureSlotMapping;

    GLenum lightingShaderStage;
};


#endif
