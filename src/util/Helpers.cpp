#include "util/Helpers.h"

#include <physfs.hpp>

std::string Helpers::readToString(const std::string& filename)
{
    PhysFS::ifstream stream(filename);
    return std::string(std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>());
}
