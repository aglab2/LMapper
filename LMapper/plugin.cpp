#include "plugin.h"

#include "Mapper/MappingImpl.h"
#include "Win.h"

#include <filesystem>

void Plugin::ReadConfig()
{
    cfg = {};

    auto cfgPath = Win::ConfigPath();
    if (!std::filesystem::exists(cfgPath))
    {
        auto errorMsg = "Path '" + cfgPath + "' does not exist!";
        MessageBox(NULL, errorMsg.c_str(), "Mapper", MB_OK);
        return;
    }

    try
    {
        auto node = YAML::LoadFile(cfgPath);
        cfg = node.as<Config>();
    }
    catch (std::exception& ex)
    {
        auto errorMsg = "Failed to read cfg from '" + cfgPath + "': " + ex.what();
        MessageBox(NULL, errorMsg.c_str(), "Mapper", MB_OK);
        return;
    }
    catch (...)
    {
        auto errorMsg = "Failed to read cfg from '" + cfgPath + "'";
        MessageBox(NULL, errorMsg.c_str(), "Mapper", MB_OK);
        return;
    }
}

void Plugin::Get(int controller, BUTTONS* buttons)
{
    *buttons = {};
    XINPUT_STATE state;
    auto err = XInputGetState(controller, &state);
    if (ERROR_SUCCESS != err)
    {
        state = {};
    }

    auto& x360 = state.Gamepad;
    Mapping::Map(cfg.mappers, x360, activeKeys, *buttons);
}
