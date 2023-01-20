#ifndef XYMENU_XYMENU_HPP
#define XYMENU_XYMENU_HPP

#include <string>
#include <vector>

#include <CTRPluginFramework.hpp>


// Original: https://github.com/44670/NTR/
// This file is part of https://github.com/HidegonSan/XYMenu
// (C) 2023 Hidegon
// License: GPL-2.0

namespace CTRPluginFramework {
    namespace XYMenu {


        using FuncPointer = void(*)(void);

        struct MenuItem {
            std::string name;
            std::string note;
            FuncPointer func;
            bool enabled;
        };

        void DrawNTRFontChar(const Screen &screen, u8 letter, u32 x, u32 y, const Color &foreground, const Color &background);
        void DrawNTRFont(const Screen &screen, const std::string &text, u32 x, u32 y, const Color &foreground, const Color &background, bool new_line);
        void Message(const std::string &message);


        class Menu {
            public:
                // Constructor
                Menu(void);

                // Destructor
                ~Menu(void);

                /**
                 * @brief      HotKey to open the menu
                 *
                 */
                u32 HotKey;

                /**
                 * @brief      Entry label
                 *
                 * @param name Label name
                 */
                void Entry(const std::string &name);

                /**
                 * @brief      Entry label with note
                 *
                 * @param name Label name
                 * @param note Note
                 */
                void Entry(const std::string &name, const std::string &note);

                /**
                 * @brief      Entry cheat
                 *
                 * @param name Cheat name
                 * @param func Function to execute
                 */
                void Entry(const std::string &name, FuncPointer func);

                /**
                 * @brief      Entry cheat with note
                 *
                 * @param name Cheat name
                 * @param note Note
                 * @param func Function to execute
                 */
                void Entry(const std::string &name, const std::string &note, FuncPointer func);

                /**
                 * @brief      Run the menu
                 *
                 */
                void Run(void);


            private:
                std::vector<MenuItem> _entries;
                bool _is_opened;
                int _selecting_index;
                void _Update(void);
                void _Draw(void);
                void _Exec(void);

        }; // End of Menu class


    } // End of XYMenu
} // End of CTRPluginFramework


#endif
