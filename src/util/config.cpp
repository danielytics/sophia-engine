
#include "util/config.h"
#include "util/logging.h"

#include <iostream>

const std::map<Config::Error, std::string> Config::error_names{
    {Config::NotScalar, "Not Scalar"},
    {Config::NotSequence, "Not A Sequence"},
    {Config::NotMap, "Not A Map"},
    {Config::BadTypeConversion, "Bad Type Conversion"},
    {Config::AttributeMissing, "Attribute Missing"},
    {Config::InvalidValue, "Invalid Or Unexpected Value"},
    {Config::BadParse, "Failed To Parse"}
};

void Config::detail::default_error_handler (const Config::Error err, const std::string& attribute_name, const YAML::Node& node)
{
    auto error_name = Config::error_names.at(err);
    debug("Could not parse config\nError: {}\nAttribute: {}\nValue: {}\n----------", error_name, attribute_name, node);
    warn("Parse error [{}] on attribute: {}", error_name, attribute_name);
}
