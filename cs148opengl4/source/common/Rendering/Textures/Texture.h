#pragma once

#ifndef __TEXTURE__
#define __TEXTURE__

#include "common/common.h"

/*! \brief Stores the assignment framework's representation of a texture and allows reuse of the underlying OpenGL texture.
 *
 *  Note that this class does not handle loading the texture. To see how a texture is loaded, look at TextureLoader::LoadTexture().
 */
class Texture: public std::enable_shared_from_this<Texture>
{
public:
    /*! \brief Constructs a texture object.
     *  \param rawData The raw pointer to the pixel RGBA data.
     *  \param width The width of the texture.
     *  \param height The height of the texture.
     *
     *  Takes in the raw pixel data for a texture and creates an RGBA texture out of it. Note that only RGBA textures are currently
     *  supported. Additionally, we assume the data is stored such it describes the texture from bottom to top, one row at a time (aka
     *  rows are contiguous in memory, beginning from the bottom row).
     */
    Texture(GLubyte* rawData, int width, int height);

    /*! \brief Destructor.
     */
    virtual ~Texture();
    
    /*! \brief Binds the texture to a texture unit and sets the target as GL_TEXTURE_2D.
     *  \param unit The texture unit to bind to.
     */
    virtual void BeginRender(int unit) const;

    /*! \brief Unbinds the texture.
     */
    virtual void EndRender() const;
private:
    int texWidth;
    int texHeight;

    GLuint glTexture;
};

#endif
