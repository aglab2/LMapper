#include "X360Controller.h"
#include "ControllerInterfaceImpl.h"
#include "Keyboard.h"
#include "SerializationImpl.h"

namespace YAML
{
    const std::map<std::string, X360::Thumbs> convert<X360::Thumbs>::names
    {
#define ENUMSTR(name) { #name, X360::Thumbs::name } 
        ENUMSTR(LeftX),
        ENUMSTR(LeftY),
        ENUMSTR(RightX),
        ENUMSTR(RightY),
#undef ENUMSTR
    };

    const std::map<std::string, X360::Triggers> convert<X360::Triggers>::names
    {
#define ENUMSTR(name) { #name, X360::Triggers::name } 
        ENUMSTR(LeftTrigger),
        ENUMSTR(RightTrigger),
#undef ENUMSTR
    };

    const std::map<std::string, X360::Buttons> convert<X360::Buttons>::names
    {
#define ENUMSTR(name) { #name, X360::Buttons::name } 
        ENUMSTR(DpadUp),
        ENUMSTR(DpadDown),
        ENUMSTR(DpadLeft),
        ENUMSTR(DpadRight),
        ENUMSTR(Start),
        ENUMSTR(Back),
        ENUMSTR(LeftThumb),
        ENUMSTR(RightThumb),
        ENUMSTR(L),
        ENUMSTR(R),
        ENUMSTR(Guide),
        ENUMSTR(A),
        ENUMSTR(B),
        ENUMSTR(X),
        ENUMSTR(Y),
#undef ENUMSTR
    };

    // TODO: Avoid repetition?
    Node convert<X360::Thumbs>::encode(const enum X360::Thumbs& thumb)
    {
        return Serializer::Encode(names, thumb);
    }

    bool convert<X360::Thumbs>::decode(const Node& node, enum X360::Thumbs& thumb)
    {
        return Serializer::Decode(names, node, thumb);
    }

    bool convert<X360::Thumbs>::is(std::string& name)
    {
        return names.find(name) != names.end();
    }

    Node convert<X360::Triggers>::encode(const enum X360::Triggers& thumb)
    {
        return Serializer::Encode(names, thumb);
    }

    bool convert<X360::Triggers>::decode(const Node& node, enum X360::Triggers& thumb)
    {
        return Serializer::Decode(names, node, thumb);
    }

    bool convert<X360::Triggers>::is(std::string& name)
    {
        return names.find(name) != names.end();
    }

    Node convert<X360::Buttons>::encode(const enum X360::Buttons& thumb)
    {
        return Serializer::EncodeBitWise(names, thumb);
    }

    bool convert<X360::Buttons>::decode(const Node& node, enum X360::Buttons& thumb)
    {
        return Serializer::DecodeBitWise(names, node, thumb);
    }

    Node convert<X360::IEventPtr>::encode(const X360::IEventPtr& ptr)
    {
        return ptr->Serialize();
    }

    bool convert<X360::IEventPtr>::decode(const Node& node, X360::IEventPtr& ptr)
    {
        if (node.IsScalar() || node.IsSequence())
        {
            // Button case
            bool isXboxButton = true;
            if (node.IsScalar())
            {
                auto name = node.as<std::string>();
                if (0 == name.compare(0, 3, "Key"))
                {
                    isXboxButton = false;
                }
            }

            if (isXboxButton)
            {
                auto buttons = node.as<X360::Buttons>();
                ptr = std::make_shared<X360::Button>(buttons);
            }
            else
            {
                auto buttons = node.as<Keyboard::Buttons>();
                ptr = std::make_shared<Keyboard::Button>(buttons);
            }
            return true;
        }

        if (node.IsMap())
        {
            auto typeNode = node["type"];
            if (!typeNode)
                return false;

            auto type = typeNode.as<std::string>();
            if (type == "axis")
            {
                auto offsetNode = node["offset"];
                if (!offsetNode)
                    return false;

                auto offsetStr = offsetNode.as<std::string>();
                if (convert<X360::Thumbs>::is(offsetStr))
                {
                    ptr = node.as<X360::ThumbPtr>();
                    return true;
                }
                else if (convert<X360::Triggers>::is(offsetStr))
                {
                    ptr = node.as<X360::TriggerPtr>();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    bool convert<X360::ThumbPtr>::decode(const Node& node, X360::ThumbPtr& ptr)
    {
        if (!node.IsMap())
            return false;

        auto offsetNode = node["offset"];
        auto valueNode = node["axis"];
        auto comparNode = node["comparer"];

        if (!offsetNode || !valueNode || !comparNode)
            return false;

        auto offset = offsetNode.as<X360::Thumbs>();
        auto value = (SHORT)valueNode.as<int>();
        auto compar = comparNode.as<ControllerInterface::AxisComparerType>();

        ptr = std::make_shared<X360::Thumb>(offset, compar, value);
        return true;
    }

    bool convert<X360::TriggerPtr>::decode(const Node& node, X360::TriggerPtr& ptr)
    {
        if (!node.IsMap())
            return false;

        auto offsetNode = node["offset"];
        auto valueNode = node["axis"];
        auto comparNode = node["comparer"];

        if (!offsetNode || !valueNode || !comparNode)
            return false;

        auto offset = offsetNode.as<X360::Triggers>();
        auto value = (BYTE)valueNode.as<int>();
        auto compar = comparNode.as<ControllerInterface::AxisComparerType>();

        ptr = std::make_shared<X360::Trigger>(offset, compar, value);
        return true;
    }
}

namespace X360
{
    Button::Button(Buttons button) : IButton(button) { }

    bool Button::Happened(const Controller& c, const std::atomic_bool*) const
    {
        return Applied(c.wButtons);
    }

    std::string Button::ToString()
    {
        return "";
    }

    template<typename AxisT, typename OffsetT>
    Axis<AxisT, OffsetT>::Axis(IAxis<AxisT, OffsetT> me) : IAxis<AxisT, OffsetT>(me) { }

    template<typename AxisT, typename OffsetT>
    bool Axis<AxisT, OffsetT>::Happened(const Controller& c, const std::atomic_bool*) const
    {
        return IAxis<AxisT, OffsetT>::Applied(&c);
    }

    template<typename AxisT, typename OffsetT>
    std::string Axis<AxisT, OffsetT>::ToString()
    {
        return "";
    }

    // TODO: Stupid!
    Thumb::Thumb(Thumbs thumb, ControllerInterface::AxisComparerType compar, SHORT value) : Axis(Intf(value, compar, thumb)) { }
    Trigger::Trigger(Triggers trigger, ControllerInterface::AxisComparerType compar, BYTE value) : Axis(Intf(value, compar, trigger)) { }
}
