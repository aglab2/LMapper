#pragma once

#include <Windows.h>
#include "Controller #1.1.h"
#include "config.h"

class Plugin
{
public:
    void ReadConfig();
    void Get(int controller, BUTTONS*);

    const Mapping::Mappers& Mappers() const { return cfg.mappers; }
    const std::string& ConfigPath() const { return configPath; }
    bool savestateHack() const { return cfg.savestateHack; }

private:
    std::string configPath;
    Config cfg;
};
