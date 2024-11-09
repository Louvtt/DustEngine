//
// Created by louvtt on 9/19/24.
//

#include "dust/io/resourceManager.hpp"

dust::io::ResourceManager::ResourceManager()
: resources(), totalCount(0) {}

dust::io::Resource::Handle dust::io::ResourceManager::registerResource(Resource *resource) {
    resources.push_back(resource);
    return totalCount++;
}

void dust::io::ResourceManager::update() {
    for(auto resource : resources) {
        if(resource) resource->update();
    }
}

