#include "config.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
    Node convert<Config>::encode(const Config& me)
    {
        Node node;
        node["controller"] = YAML::Node(me.mappers);
        if (me.enabledControllersMask & Config::AllowedEnabledControllerMask)
        {
            YAML::Node enabledControllers;
            for (int i = 0; i < 4; i++)
            {
                if (me.enabledControllersMask & (1 << i))
                {
                    enabledControllers.push_back(1 + i);
                }
            }
            node["enabled"] = enabledControllers;
        }
        {
            YAML::Node hacks;
            if (me.savestateHack)
                hacks.push_back("savestate");

            node["hacks"] = std::move(hacks);
        }
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

            if (auto hacksNode = node["hacks"])
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

            if (auto enabledNode = node["enabled"])
            {
                if (enabledNode.IsScalar())
                {
                    if (int enabledVal = enabledNode.as<int>())
                        cfg.enabledControllersMask |= 1 << (enabledVal - 1);
                    else
                        return false;
                }
                else if (enabledNode.IsSequence())
                {
                    for (const auto& enabledControllerNode : enabledNode)
                    {
                        if (!enabledControllerNode.IsScalar())
                            return false;

                        if (int enabledVal = enabledControllerNode.as<int>())
                            cfg.enabledControllersMask |= 1 << (enabledVal - 1);
                        else
                            return false;
                    }
                }
                else
                {
                    return false;
                }

                cfg.enabledControllersMask &= Config::AllowedEnabledControllerMask;
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
