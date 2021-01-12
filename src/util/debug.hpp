#pragma once

// Collection of things useful for debugging

namespace MjB {

template <typename CONDITION, typename... MSGS>
inline void debugAssert(CONDITION condition, MSGS const&... msgs) {
#ifndef NDEBUG
    if (not condition) {
        
    }
#endif // NDEBUG
}

} // namespace MjB
