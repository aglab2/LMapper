#include "Group.h"
#include "SerializationImpl.h"

namespace Mapping
{
    Group::Group(std::vector<IMapperPtr> m)
        : mappers_(std::move(m))
    { }

    std::string Group::ToString()
    {
        return {};
    }

    void Group::Map(InputFactory& factory, N64::Controller& to)
    {
        for (auto& ev : mappers_)
        {
            ev->Map(factory, to);
        }
    }

    YAML::Node Group::Serialize() const
    {
        YAML::Node grp;
        for (auto& mapper : mappers_)
        {
            grp.push_back(mapper->Serialize());
        }

        YAML::Node ret;
        ret["group"] = std::move(grp);
        ret["type"] = "group";
        return ret;
    }
}

namespace YAML
{
    bool convert<Mapping::GroupPtr>::decode(const Node& node, Mapping::GroupPtr& s)
    {
        auto groupNode = node["group"];
        if (!groupNode)
            return false;

        auto group = groupNode.as<std::vector<Mapping::IMapperPtr>>();

        s = std::make_shared<Mapping::Group>(group);
        return true;
    }
}