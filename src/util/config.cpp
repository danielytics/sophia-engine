
#include "util/config.h"

#include <iostream>

const std::map<Config::Error, std::string> Config::error_names{
    {Config::NotScalar, "Not Scalar"},
    {Config::NotSequence, "Not A Sequence"},
    {Config::NotMap, "Not A Map"},
    {Config::BadParse, "Bad Parse"},
    {Config::BadTypeConversion, "Bad Type Conversion"},
    {Config::AttributeMissing, "Attribute Missing"}
};

void Config::detail::default_error_handler (const Config::Error err, const std::string& name, const YAML::Node& node)
{
    auto error = Config::error_names.at(err);
    std::cerr << "Parse error [" << error << "] on attribute: " << name << "\n" << node << "\n----------\n";
};
