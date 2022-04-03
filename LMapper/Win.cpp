#include "Win.h"

#include <Windows.h>

namespace Win
{
    std::string ExecutablePath()
    {
        int count = 128;
        std::string path;
        do
        {
            count *= 2;
            path = std::string(count, 0);
        }
        while (0 == GetModuleFileName(NULL, path.data(), count));

        return path.c_str();
    }
}
