#include "plugin.h"

#include "Mapper/MappingImpl.h"
#include "Win.h"

#include <filesystem>

void Plugin::ReadConfig()
{
    cfg = {};

    std::filesystem::path path(Win::ExecutablePath());
    auto cfgPath = path.remove_filename() / "Config" / "map.yaml";
    configPath = cfgPath.string();
    if (!std::filesystem::exists(cfgPath))
    {
        auto errorMsg = "Path '" + cfgPath.string() + "' does not exist!";
        MessageBox(NULL, errorMsg.c_str(), "Mapper", MB_OK);
        return;
    }

    try
    {
        auto node = YAML::LoadFile(cfgPath.u8string());
        cfg = node.as<Config>();
    }
    catch (std::exception& ex)
    {
        auto errorMsg = "Failed to read cfg from '" + cfgPath.string() + "': " + ex.what();
        MessageBox(NULL, errorMsg.c_str(), "Mapper", MB_OK);
        return;
    }
    catch (...)
    {
        auto errorMsg = "Failed to read cfg from '" + cfgPath.string() + "'";
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
