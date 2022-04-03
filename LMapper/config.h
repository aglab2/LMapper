#pragma once

#include "Mapper/Mapping.h"

class Config
{
public:
    Mapping::Mappers mappers;
    bool savestateHack = false;
};

namespace YAML
{
    template<>
    struct convert<Config>
    {
        static Node encode(const Config&);
        static bool decode(const Node& node, Config&);
    };
}