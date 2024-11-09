//
// Created by louvtt on 9/19/24.
//

#ifndef _DUST_IO_RESOURCEMANAGER_HPP_
#define _DUST_IO_RESOURCEMANAGER_HPP_

#include "resource.hpp"

#include <vector>

namespace dust {
    namespace io {
        class ResourceManager {
        private:
            std::vector<Resource *> resources;
            Resource::Handle totalCount;

        public:
            ResourceManager();

            ~ResourceManager() = default;

            Resource::Handle registerResource(Resource *resource);

            void update();
        };
    }
}

#endif //_DUST_IO_RESOURCEMANAGER_HPP_
