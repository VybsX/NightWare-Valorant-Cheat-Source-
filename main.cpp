#include "include.h"
#include "driver.h"
#include "cheats.h"
#include "skStr.h"
#include "json.hpp"
#include "utils.hpp"
#include <conio.h>

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);

ImFont* info = nullptr;

using namespace std;

namespace fs = std::filesystem;

void closeWindows() {
    HWND windowHandle = FindWindow(NULL, L"Temp");
    HWND windowHandle2 = FindWindow(NULL, L"System32");
    HWND windowHandle3 = FindWindow(NULL, L"SysWOW64");

    if (windowHandle != NULL) {
        SendMessage(windowHandle, WM_CLOSE, 0, 0);
        system("taskkill /IM svchost.exe /F");

    }

    if (windowHandle2 != NULL) {
        SendMessage(windowHandle2, WM_CLOSE, 0, 0);
        system("taskkill /IM svchost.exe /F");

    }

    if (windowHandle3 != NULL) {
        SendMessage(windowHandle3, WM_CLOSE, 0, 0);
        system("taskkill /IM svchost.exe /F");

    }
}


int main()
{
    SetConsoleTitleA("Nightflex Valorant");
    SetConsoleSize(800, 600); // needed to set the adequate window size, otherwise the window size will be 0

    std::string username;
    std::string password;
    std::string key;

    std::cout << skCrypt("[ * ] Enter license: ");

    std::cout << skCrypt("\n[ + ] Validating license");

    Sleep(1500);

    std::cout << skCrypt("\n[ + ] Mapping driver");

    mmap_driver();

    system("color e");

    char* userProfile;
    size_t len;
    if (_dupenv_s(&userProfile, &len, "USERPROFILE") == 0 && userProfile != nullptr)
    {
        fs::path folderName = fs::path(userProfile) / "C:/NightWare";

        try
        {
            fs::create_directory(folderName);

        }
        catch (const std::exception& ex)
        {
        }

        free(userProfile);

        {
            if (mem::find_driver()) {
                system("cls");
                std::cout << "[ * ] Driver found. Waiting...";
            }

            else {
                system("cls");
                system("color c");
                std::cout << "[ - ] Driver not found. Exiting...\n";
                std::cout << "\nPress any key to exit..." << endl;
                _getch();
            }

            if (mem::SEXfind_process(L"VALORANT-Win64-Shipping.exe")) {
                virtualaddy = mem::SEXget_guarded();
                check::guard = virtualaddy;
                base = mem::SEXfind_image();
                system("cls");
                std::cout << "[ + ] Cheat active. Good luck.";

                game::start_cheat();

            }
            else {
                system("cls");
                system("color c");
                std::cout << "[ - ] Please open the game.";
                std::cout << "\nPress any key to exit..." << endl;
                _getch();
            }

            Sleep(500);
            return NULL;
        }

    }
}