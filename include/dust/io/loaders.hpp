#ifndef _DUST_IO_IMAGELOADER_HPP_
#define _DUST_IO_IMAGELOADER_HPP_

#include "dust/io/assetsManager.hpp"

#include "dust/render/material.hpp"
#include "dust/render/model.hpp"
#include "dust/render/texture.hpp"


using namespace dust;
namespace dr = dust::render;

DUST_ADD_LOADER_SYNC_DEF_EX(dr::TextureDesc, bool mipMaps);

DUST_ADD_LOADER_SYNC_DEF(dr::ModelPtr);
DUST_ADD_LOADER_ASYNC_DEF(dr::ModelPtr);

DUST_ADD_LOADER_SYNC_DEF(std::string);


#endif //_DUST_IO_IMAGELOADER_HPP_