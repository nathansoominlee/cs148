#pragma once

#ifndef __TEXTURE_LOADER__
#define __TEXTURE_LOADER__

#include "common/common.h"

class Texture;

namespace TextureLoader
{

std::shared_ptr<Texture> LoadTexture(const std::string& filename);

}
#endif
