#ifndef _DUST_CORE_TIME_HPP_
#define _DUST_CORE_TIME_HPP_

#include "types.hpp"

namespace dust {

struct Time
{
    f64 delta;
    f64 time;
    u64 frame;
};

}

#endif //_DUST_CORE_TIME_HPP_