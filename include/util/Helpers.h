#ifndef HELPERS_H
#define HELPERS_H

#include <utility>

namespace Helpers {

template <class ContainerType>
bool remove(ContainerType& container, std::size_t index)
{
    auto it = container.begin() + index;
    auto last = container.end() - 1;
    if (it != last) {
        // If not the last item, move the last into this element
        *it = std::move(*last);
    }
    // Remove the last item in the container
    container.pop_back();
}

}

#endif // HELPERS_H
