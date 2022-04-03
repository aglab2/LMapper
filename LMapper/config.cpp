#include "config.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
    Node convert<Config>::encode(const Config& me)
    {
        Node node;
        node["controller"] = YAML::Node(me.mappers);
        YAML::Node hacks;
        if (me.savestateHack)
            hacks.push_back("savestate");

        node["hacks"] = std::move(hacks);
        return node;
    }

    bool convert<Config>::decode(const Node& node, Config& cfg)
    {
        if (node.IsMap())
        {
            auto controllerNode = node["controller"];
            if (!controllerNode)
                return false;
            
            cfg.mappers = controllerNode.as<Mapping::Mappers>();

            auto hacksNode = node["hacks"];
            if (hacksNode)
            {
                auto hacks = hacksNode.as<std::vector<std::string>>();
                for (auto& hack : hacks)
                {
                    if ("savestate" == hack)
                    {
                        cfg.savestateHack = true;
                    }
                }
            }

            return true;
        }
        else
        {
            cfg.mappers = node.as<Mapping::Mappers>();
            return true;
        }
    }
}
