#ifndef HELPERS_H
#define HELPERS_H

#include <utility>

namespace Helpers {

template <typename ContainerType>
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

template <typename ContainerType>
void move_back_and_replace(ContainerType& container, std::size_t index, typename ContainerType::reference&& data) {
    container.push_back(std::move(container[index]));
    container[index] = std::move(data);
}

}

#endif // HELPERS_H
