#ifndef LIB_H
#define LIB_H

#ifdef USE_EASTL

#include <EASTL/vector.h>
#include <EASTL/atomic.h>

namespace lib = eastl;

#else

#include <vector>
#include <atomic>

namespace lib = std;

#endif


class Lock {
lib::atomic_uint counter;
public:
template <typename Fn>
static bool use (Lock& lock, Fn& fn) {
    bool used = false;
    if (lock.fetch_add(1) == 0) {
        fn();
        used = true;
    }
    lock.counter.fetch_sub(1);
    return used;
}
};

#endif // LIB_H
