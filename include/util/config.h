#ifndef CONFIG_H
#define CONFIG_H

#include <yaml-cpp/yaml.h>

#include <functional>
#include <string>

namespace Config {
    typedef const std::function<void(const YAML::Node&)> Def;

    struct Wrapper {
        const Def parse;
    };
    typedef std::vector<Def> Defs;

    namespace detail {
        template <typename... Args>
        void def (Defs& definitions, Def& definition, Args... defs) {
            def(definitions, definition);
            def(definitions, defs...);
        }

        template <>
        void def (Defs& definitions, Def& definition) {
            definitions.push_back(definition);
        }
    }


    template <typename... Definitions>
    Def make_parser (Definitions... defs) {
        Defs def_fns = Defs{};
        detail::def(def_fns, defs...);
        return [def_fns](const YAML::Node& node) {
            for (auto fn : def_fns) {
                fn(node);
            }
        };
    }

    template <typename Type>
    Def scalar (const std::string& name, Type& output) {
        return [name, &output](const YAML::Node& input) mutable {
            YAML::Node node = input[name];
            if (node.IsScalar()) {
                output = node.as<Type>();
            }
        };
    }

    template <typename Type>
    Def sequence (const std::string& name, Type& output) {
        return [name, &output](const YAML::Node& input) mutable {
            YAML::Node node = input[name];
            if (node.IsSequence()) {
                output = node.as<Type>();
            }
        };
    }
    template <typename Type>
    Def sequence (Type& output) {
        return [&output](const YAML::Node& input) mutable {
            if (input.IsSequence()) {
                output = input.as<Type>();
            }
        };
    }
    template <typename Type, typename... Children>
    Def sequence (const std::string& name, Type& temp, std::vector<Type>& output, Children... children) {
        auto parse = make_parser(children...);
        return [name, parse, &temp, &output](const YAML::Node& input) mutable {
            YAML::Node node = input[name];
            if (node.IsSequence()) {
                for (auto child : node) {
                    parse(child);
                    output.push_back(temp);
                }
            }
        };
    }

    template <typename... Children>
    Def map (const std::string& name, Children... children) {
        auto parse = make_parser(children...);
        return [name, parse](const YAML::Node& input) mutable {
            YAML::Node node = input[name];
            if (node.IsMap()) {
                parse(node);
            }
        };
    }
}
#endif // CONFIG_H
