#ifndef LIB_H
#define LIB_H

#ifdef USE_EASTL
/**
  * EASTL should be used instead of C++ Standard Library for containers.
  * std::string, std::fstream, std::atomic and others are still used (and are not aliased to the lib namespace).
  */

#include <EASTL/vector.h>
#include <EASTL/map.h>
#include <EASTL/algorithm.h>

namespace lib = eastl;

#else

#include <vector>
#include <map>
#include <algorithm>

namespace lib = std;

#endif

#include <atomic>

class Lock {
    std::atomic_uint counter;
public:
    template <typename Fn>
    static bool use (Lock& lock, Fn& fn) {
        bool used = false;
        if (lock.counter.fetch_add(1) == 0) {
            fn();
            used = true;
        }
        lock.counter.fetch_sub(1);
        return used;
    }
};

#endif // LIB_H
