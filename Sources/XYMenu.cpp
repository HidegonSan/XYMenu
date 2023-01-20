#include "CTRPluginFramework.hpp"

#include "CTRPluginFramework/Graphics/OSD.hpp"
#include "CTRPluginFramework/System/Controller.hpp"
#include "CTRPluginFramework/System/Process.hpp"

#include "XYMenu/XYMenu.hpp"
#include "XYMenu/font.h"

#include <cstddef>
#include <string>


// Original: https://github.com/44670/NTR/
// This file is part of https://github.com/HidegonSan/XYMenu
// (C) 2023 Hidegon
// License: GPL-2.0

namespace CTRPluginFramework {
    namespace XYMenu {


        // Start of utility function
        void DrawNTRFontChar(const Screen &screen, u8 letter, u32 x, u32 y, const Color &foreground, const Color &background) {

            unsigned char mask = 0b10000000;
            unsigned char l;

            if ((letter < 32) || (letter > 127)) { // Out of range
                letter = '?';
            }

            int font_index = (letter - 32)*12; // 32 = \u0000 ~ \u0020. \u0021(33) = ! (first visible char)

            for (int yy=0; yy<12; ++yy) {
                l = g_font[font_index + yy]; // buffer of char (row)
                for (int xx=0; xx<8; ++xx) {
                    if ((mask >> xx) & l){ // foreground (0b1=foreground)
                        screen.DrawPixel(x + xx, y + yy, foreground);
                    }
                    else { // background
                        screen.DrawPixel(x + xx, y + yy, background);
                    }
                }
            }
        }


        void DrawNTRFont(const Screen &screen, const std::string &text, u32 x, u32 y, const Color &foreground, const Color &background, bool new_line) {
            u32 tmp_x = x;
            u32 line = 0;

            for (int i=0; i<text.length(); ++i) {
                if (new_line && (320 < (tmp_x + 8))) { // New line. 320 = Bottom screen width
                    ++line;
                    tmp_x = x;
                }

                DrawNTRFontChar(screen, text[i], tmp_x, y + (line*8), foreground, background);

                tmp_x += 8; // 8 = char width
            }
        }


        void Message(const std::string &message) {
            const Screen &screen = OSD::GetBottomScreen();

            if (!Process::IsPaused()) {
                Process::Pause();
            }

            while (true) {
                Sleep(Milliseconds(16));
                Controller::Update();

                if (Controller::IsKeysPressed(Key::B)) { // close
                    Process::Play();
                    break;
                }

                screen.DrawRect(0, 0, 320, 240, Color::White);
                DrawNTRFont(screen, message, 10, 10, Color::Red, Color::White, true);
                DrawNTRFont(screen, "Press [B] to close.", 10, 220, Color::Blue, Color::White, false);
                OSD::SwapBuffers();
            }
            return;
        }
        // End of utility function


        // Start of Menu class
        // Constructor
        Menu::Menu() : HotKey(Key::X | Key::Y), _entries({}), _is_opened(false), _selecting_index(0) {}


        // Destructor
        Menu::~Menu() {
            if (this->_is_opened) {
                this->_is_opened = false;
                Process::Play();
            }
        }


        // Entry label
        void Menu::Entry(const std::string &name) {
            this->_entries.push_back({ name, "", nullptr, false });
        }


        // Entry label with note
        void Menu::Entry(const std::string &name, const std::string &note) {
            this->_entries.push_back({ name, note, nullptr, false});
        }


        // Entry cheat
        void Menu::Entry(const std::string &name, FuncPointer func) {
            this->_entries.push_back({ name, "", func, false});
        }


        // Entry cheat with note
        void Menu::Entry(const std::string &name, const std::string &note, FuncPointer func) {
            this->_entries.push_back({ name, note, func, false});
        }


        // Run the menu
        void Menu::Run(void) {
            Sleep(Milliseconds(16));
            Controller::Update();

            // Open the menu
            if (!this->_is_opened && (Controller::IsKeysPressed(this->HotKey))) {
                this->_is_opened = true;
                Process::Pause();
            }

            // Menu is opened
            while (this->_is_opened) {
                Controller::Update();

                // Close the menu
                if (Controller::IsKeysPressed(Key::B)) {
                    this->_is_opened = false;
                    Process::Play();
                    break;
                }

                this->_Draw();
                this->_Update();

                OSD::SwapBuffers();
            }

            // Exec enabled cheats
            this->_Exec();

        } // End of Menu::Run


        // Process key event
        void Menu::_Update(void) {
            const int entry_count = this->_entries.size();
            const u32 pressed_key = Controller::GetKeysPressed();

            if (pressed_key & Key::DPadUp) {
                this->_selecting_index--;
                if (this->_selecting_index < 0) {
                    this->_selecting_index = entry_count - 1;
                }
            }
            else if (pressed_key & Key::DPadDown) {
                this->_selecting_index++;
                if (entry_count <= (this->_selecting_index)) {
                    this->_selecting_index = 0;
                }
            }
            else if (pressed_key & Key::A) {
                if (this->_entries[this->_selecting_index].func != nullptr) {
                    this->_entries[this->_selecting_index].enabled = !this->_entries[this->_selecting_index].enabled;
                }
            }
        } // End of Menu::_Update


        // Draw the menu
        void Menu::_Draw(void) {
            const Screen &screen = OSD::GetBottomScreen();
            const int entry_count = this->_entries.size();

            screen.DrawRect(0, 0, 320, 240, Color::White);
            DrawNTRFont(screen, "Game Plugin Config", 10, 10, Color::Red, Color::White, false);
            DrawNTRFont(screen, /* "http://44670.org/ntr" */ "https://github.com/HidegonSan/XYMenu", 10, 220, Color::Blue, Color::White, false);

            int draw_start_index = (this->_selecting_index / 10)*10; // 10 = Max captions
            int draw_end_index = draw_start_index + 10;
            if (entry_count < draw_end_index) {
                draw_end_index = entry_count;
            }
            int y = 30;

            // Draw entries
            for (int i=draw_start_index; i<draw_end_index; ++i) {

                MenuItem entry = this->_entries[i];

                std::string name = "";
                name += ((i == this->_selecting_index) ? " * " : "   "); // Selecting
                if (entry.func != nullptr) { // Cheat
                    name += (entry.enabled ? "[X] " : "[ ] ");
                }
                else { // Label
                    name += "    ";
                }
                name += entry.name;

                DrawNTRFont(screen, name, 10, y, Color::Black, Color::White, false);

                y += 13;
            }

            DrawNTRFont(screen, this->_entries[this->_selecting_index].note, 10, y, Color::Blue, Color::White, true); // Draw note
        } // End of Menu::_Draw


        // Exec enabled cheats
        void Menu::_Exec(void) {
            for (auto &&entry : this->_entries) {
                if (entry.func != nullptr && entry.enabled) {
                    entry.func();
                }
            }
        } // End of Menu::_Exec


    } // End of XYMenu
} // End of CTRPluginFramework
