#ifndef _DUST_IO_IMAGELOADER_HPP_
#define _DUST_IO_IMAGELOADER_HPP_

#include "dust/io/assetsManager.hpp"

#include "dust/render/material.hpp"
#include "dust/render/model.hpp"
#include "dust/render/texture.hpp"


using namespace dust;
namespace dr = dust::render;

namespace dust::io {

DUST_ADD_LOADER(dr::TexturePtr, Texture2D);

DUST_ADD_LOADER(dr::ModelPtr, Model);
DUST_ADD_LOADER_ASYNC(dr::ModelPtr, Model);

DUST_ADD_LOADER(std::string, File);

}


#endif //_DUST_IO_IMAGELOADER_HPP_