//
// Created by louvtt on 7/23/24.
//

#include "dust/io/resource.hpp"
#include "dust/core/application.hpp"

dust::io::Resource::Resource() {
    handle = Application::Get()->getResourceManager()->registerResource(this);
}

uint32_t dust::io::Resource::getHandle() const {
    return handle;
}

void dust::io::Resource::update() { }
