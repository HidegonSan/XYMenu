#include <3ds.h>
#include "3ds/services/apt.h"
#include "CTRPluginFramework/Menu/PluginMenu.hpp"
#include "CTRPluginFramework/System/Sleep.hpp"
#include "CTRPluginFramework/System/Time.hpp"
#include "csvc.h"
#include <CTRPluginFramework.hpp>

#include "XYMenu/XYMenu.hpp"
#include "cheats.hpp"
#include <vector>


namespace CTRPluginFramework {


    // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
    static void    ToggleTouchscreenForceOn(void) {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original) {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern = {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr) {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }


    // This function is called before main and before the game starts
    // Useful to do code edits safely
    void    PatchProcess(FwkSettings &settings) {
        ToggleTouchscreenForceOn();
    }


    // This function is called when the process exits
    // Useful to save settings, undo patchs or clean up things
    void    OnProcessExit(void) {
        ToggleTouchscreenForceOn();
    }


    void    InitMenu(XYMenu::Menu &menu) {
        // Create your entries here
        // Example entry
        menu.Entry("XY Menu Example");
        menu.Entry("About", "Made by Hidegon");
        menu.Entry("Max Money", "Set the money to 999999", MaxMoney);
    }


    int     main(void) {
        // Show HotKey
        XYMenu::Message("HotKey: X + Y");

        XYMenu::Menu *menu = new XYMenu::Menu();

        // Init our menu entries
        InitMenu(*menu);

        // Launch menu
        while (true) {
            menu->Run();
        }

        delete menu;

        // Exit plugin
        return 0;
    }


}
