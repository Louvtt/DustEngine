//
// Created by louvtt on 7/23/24.
//

#ifndef _DUST_IO_RESOURCE_HPP_
#define _DUST_IO_RESOURCE_HPP_

#include "dust/core/types.hpp"

namespace dust::io {
    class Resource {
    public:
        using Handle = uint32_t;

    private:
        Handle handle;

    public:
        explicit Resource();

        virtual ~Resource() = default;

        virtual void update();

        [[nodiscard]] Handle getHandle() const;
    };
}


#endif // _DUST_IO_RESOURCE_HPP_
