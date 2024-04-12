#pragma once

#include "Mapping.h"

#include "SerializationImpl.h"

namespace Mapping
{
    namespace Analog
    {
        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::LinearMapper(FromConverter from, ToConverter to, Deadzoner dz) : fromConverter_(from), toConverter_(to), deadzoner_(dz) { }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        std::string LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::ToString()
        {
            std::ostringstream ss;
            ss << "Linear { " << fromConverter_.ToString() << "} -> { " << toConverter_.ToString() << "}";
            return ss.str();
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        void LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Map(InputFactory& from, N64::Controller& to)
        {
            auto fromValPtr = fromConverter_.Get(&from);
            auto toValPtr = toConverter_.Get(&to);

            auto fromValConv = fromConverter_.Convert(*fromValPtr);
            if (deadzoner_)
                deadzoner_->Apply(fromValConv);
            auto toValConv = toConverter_.Convert(fromValConv);

            *toValPtr = toValConv;
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        const std::string LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::name_ = "linear";

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        YAML::Node LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Serialize() const
        {
            YAML::Node node;
            node["type"] = name_;
            node["from"] = fromConverter_;
            node["to"] = toConverter_;
            if (deadzoner_)
                node["deadzone"] = *deadzoner_;
            return node;
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        std::string BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::ToString()
        {
            std::ostringstream ss;
            ss << "Bilinear { " << fromConverters_[0].ToString() << "}x{" << fromConverters_[1].ToString() << 
                      "} -> { " << toConverters_[0].ToString()   << "}x{" << toConverters_[1].ToString()   << "}";
            return ss.str();
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        void BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Map(const X360::Controller& from, const std::atomic_bool*, N64::Controller& to)
        {
            float tmpValues[2];
            for (int i = 0; i < 2; i++)
            {
                auto& converter = fromConverters_[i];
                auto fromValPtr = converter.Get(&from);
                tmpValues[i] = converter.Convert(*fromValPtr);
            }

            if (deadzoner_)
                deadzoner_->Apply(tmpValues);
            if (bilinearDeadzoner_)
                bilinearDeadzoner_->Apply(tmpValues);
            if (stretcher_)
                stretcher_->Stretch(tmpValues[0], tmpValues[1]);
            if (angleDeadzone_)
                angleDeadzone_->Apply(tmpValues);

            for (int i = 0; i < 2; i++)
            {
                auto& converter = toConverters_[i];
                auto toValPtr = converter.Get(&to);
                *toValPtr = converter.Convert(tmpValues[i]);
            }
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::BilinearMapper(FromConverter fX, FromConverter fY, ToConverter tX, ToConverter tY, Stretcher s, Deadzoner d, BilinearDeadzoner bd, AngleLimiter al)
            : fromConverters_{ fX, fY }, toConverters_{ tX, tY }, stretcher_(s), deadzoner_(d), bilinearDeadzoner_(bd), angleDeadzone_(al) 
        { }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        const std::string BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::name_ = "bilinear";

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        YAML::Node BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Serialize() const
        {
            YAML::Node node;
            node["type"] = name_;
            node["fromX"] = fromConverters_[0];
            node["fromY"] = fromConverters_[1];
            node["toX"] = toConverters_[0];
            node["toY"] = toConverters_[1];
            if (stretcher_)
                node["stretcher"] = *stretcher_;
            if (deadzoner_)
                node["deadzone"] = *deadzoner_;
            if (bilinearDeadzoner_)
                node["deadzone"] = *bilinearDeadzoner_;
            if (angleDeadzone_)
                node["angleDeadzone"] = *angleDeadzone_;
            return node;
        }
    }
}

namespace YAML
{
    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    bool convert<Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>::decode(const Node& node, Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>& mapper)
    {
        if (!node.IsMap())
            return true;

        auto fromNode = node["from"];
        auto toNode = node["to"];
        auto deadzoneNode = node["deadzone"];

        std::optional<ControllerInterface::Deadzoner> deadzoner;
        if (deadzoneNode)
            deadzoner = deadzoneNode.as<ControllerInterface::Deadzoner>();

        if (!fromNode || !toNode)
            return false;
        
        auto from = fromNode.as<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::FromConverter>();
        auto to = toNode.as<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::ToConverter>();

        mapper = std::make_shared<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>(from, to, deadzoner);
        return true;
    }

    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    bool convert<Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>::decode(const Node& node, Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>& mapper)
    {
        if (!node.IsMap())
            return true;

        auto fromXNode = node["fromX"];
        auto fromYNode = node["fromY"];
        auto toXNode = node["toX"];
        auto toYNode = node["toY"];
        auto stretcherNode = node["stretcher"];
        auto deadzoneNode = node["deadzone"];
        auto angleDeadzoneNode = node["angleDeadzone"];

        if (!fromXNode || !fromYNode || !toXNode || !toYNode)
            return false;

        auto fromX = fromXNode.as<X360::ThumbsConverter>();
        auto fromY = fromYNode.as<X360::ThumbsConverter>();
        auto toX = toXNode.as<N64::AxisConverter>();
        auto toY = toYNode.as<N64::AxisConverter>();
        std::optional<ControllerInterface::BilinearDiagonalStretcher> stretcher;
        if (stretcherNode)
            stretcher = stretcherNode.as<ControllerInterface::BilinearDiagonalStretcher>();

        std::optional<ControllerInterface::Deadzoner> deadzoner;
        std::optional<ControllerInterface::BilinearDeadzoner> bilinearDeadzoner;
        if (deadzoneNode)
        {
            if (deadzoneNode.IsScalar())
            {
                deadzoner = deadzoneNode.as<ControllerInterface::Deadzoner>();
            }
            else
            {
                bilinearDeadzoner = deadzoneNode.as<ControllerInterface::BilinearDeadzoner>();
            }
        }

        std::optional<ControllerInterface::AngleDeadzoner> angleDeadzone;
        if (angleDeadzoneNode)
            angleDeadzone = angleDeadzoneNode.as<ControllerInterface::AngleDeadzoner>();

        mapper = std::make_shared<Mapping::Analog::BilinearStickMapper>(fromX, fromY, toX, toY, stretcher, deadzoner, bilinearDeadzoner, angleDeadzone);
        return true;
    }
}
