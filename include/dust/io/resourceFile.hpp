//
// Created by louvtt on 9/18/24.
//

#ifndef _DUST_IO_RESOURCEFILE_HPP_
#define _DUST_IO_RESOURCEFILE_HPP_

#include "resource.hpp"

namespace dust::io {
    class ResourceFile : public Resource {
    protected:
        std::string filepath;
        uint32_t previousTimestamp;
        bool hasBeenLoaded;

    public:
        explicit ResourceFile(const std::string& filepath);

        void update() override;

        [[nodiscard]] std::string getFilepath() const;

        virtual void reload(bool first_load = false);
    };
}

#endif //_DUST_IO_RESOURCEFILE_HPP_
