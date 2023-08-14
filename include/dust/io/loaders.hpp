#ifndef _DUST_IO_IMAGELOADER_HPP_
#define _DUST_IO_IMAGELOADER_HPP_

#include "dust/render/material.hpp"
#include "dust/render/model.hpp"
#include "dust/render/texture.hpp"

namespace dust {

namespace io {
//////////////////////////

class ImageLoader
{  
public:
    static dust::render::Texture::Desc Read(const std::string &path, bool mipMaps = true);
};

//////////////////////////

class ModelLoader
{ 
public:
    static Ref<render::Model> Read(const std::string &path);
};

//////////////////////////

class FileLoader
{
public:
    static std::string Read(const std::string &path);
};

}
}

#endif //_DUST_IO_IMAGELOADER_HPP_