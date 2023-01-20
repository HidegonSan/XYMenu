#include "cheats.hpp"

namespace CTRPluginFramework {

    void MaxMoney(void) {
        // MaxMoney cheat for Yo-kai Watch Shinuchi (JPN)
        Process::Write32(0x8722BF8, 999999);
    }

}
