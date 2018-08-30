#ifndef HELPERS_H
#define HELPERS_H

#include "lib.h"
#include <string>

namespace Helpers {

template <typename ContainerType>
bool remove(ContainerType& container, std::size_t index)
{
    auto it = container.begin() + index;
    auto last = container.end() - 1;
    if (it != last) {
        // If not the last item, move the last into this element
        *it = lib::move(*last);
    }
    // Remove the last item in the container
    container.pop_back();
}

template <typename ContainerType>
void move_back_and_replace(ContainerType& container, std::size_t index, typename ContainerType::reference&& data)
{
    container.push_back(lib::move(container[index]));
    container[index] = lib::move(data);
}

// If elements in 'in' have a deleted copy ctor, then _inserter may not work when compiling with EASTL, this is a workaround
template <typename InputContainer, typename OutputContainer>
void move_back (InputContainer& in, OutputContainer& out)
{
    for (auto& item : in) {
        out.push_back(lib::move(item));
    }
}

template <typename ContainerType>
void pad_with (ContainerType& container, std::size_t size, typename ContainerType::value_type value)
{
    while (container.size() < size) {
        container.push_back(value);
    }
}

std::string readToString(const std::string& filename);

}

#endif // HELPERS_H
