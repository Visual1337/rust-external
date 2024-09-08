#include <Windows.h>
#include <iostream>
#include <string>

using namespace std::string_literals;

#include "crypt.h"
#include "driver.h"

#include "Hijack.hpp"
#include "Overlay.hpp"

#include <thread>

#include "cache.hpp"

BOOL WINAPI MyHandlerRoutine(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        globals->render_menu = false;
        globals->DrawFov = false;
    }
    return FALSE;
}

int main()
{
    SetConsoleCtrlHandler(&MyHandlerRoutine, TRUE);
    //ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED);
    system("color 01");

    SetConsoleOutputCP(CP_UTF8);

    std::wcout << u8R"(                                                                    
                                █▓██████████████████████████████████                                
                              ▓██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓███████████████████                              
                             ▓██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓███████████████████                             
                    ▒██████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓██████████████████████████▓                    
                   ▒█████████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓█████████████████████████▓                   
                  ▒█████████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████████                  
                 ▓█████████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████████                 
                ▓█████████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████████                
                █████████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████████████████████████▒               
                ▒███████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓███████████████████████▓                
                 ▒█████▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓███████████████████████▓                 
                   ███▓▒▒▒▒▒▒▒▒▓█████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████▓                  
                   ██▓▒▒▒▒▒▒▒▒████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████████████████████████▒                  
                 ░██▓▒▒▒▒▒▒▒▒███████████████▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████████▒                 
                ▒██▒▒▒▒▒▒▒▒▒█████████████████▓▓▓▓▓▓▓▓▓▓████████████████████████████▒                
                ██▒▒▒▒▒▒▒▒▒▓██████████▒▓██████▓▓▓▓▓▓▓▓██████▓▒██████████████████████                
               ▒██▒▒▒▒▒▒▒▒▒▒█████████▓   ██████▓▓▓▓▓▓██████░  ██████████████████████▓               
                ██▒▒▒▒▒▒▒▒▒▒▒█████████████████▒      ▓██████████████████████████████▒               
                ▓██▒▒▒▒▒▒▒▒▒▒▒███████████████         ▒████████████████████████████▓                
                 ▓██▒▒▒▒▒▒▒▒▒▒▒█████████████           ▒██████████████████████████▓                 
                  ▒██▒▒▒▒▒▒▒▒▒▒▒▓█████████▓    ▓████▓    ████████████████████████▓                  
                   ▒██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒    ████████    ██████████████████████▓                   
                    ░██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒      ▒████▒      ████████████████████▓                    
                     ░██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒                  ▓██████████████████▒                     
                       ██▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒                  ▓█████████████████▒                      
                        ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒░                ▒█████████████████░                       
                         ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓              ██████████████████░                        
                          ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▒        ▒▓██████████████████                          
                           ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓██████████████████                           
                            ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓██████████████████                            
                             ██▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓███████████████████                             
                              ███▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓███████████████████                              
                               ▓████████████████████████████████████▓                               
                                   ▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒░                                  
        )" << std::endl;

    printf("\n\n\n[+] Welcome to Rust Cheat\n");

    if (!IO::driver_init())
    {
        printf(SECURECRYPT("[*] driver not found\n"));
    }

    IO::process_id = IO::find_process(SECURECRYPT(L"RustClient.exe"));

    IO::get_cr3();

    image_base = IO::find_image();

    std::cout << "Got image_base base at: 0x" << image_base << std::endl;

    game_image_address = GetModuleBaseAddress(IO::process_id, L"GameAssembly.dll");
    UP_image_base = GetModuleBaseAddress(IO::process_id, L"UnityPlayer.dll");

    std::cout << "Got Game Assembly base at: 0x" << game_image_address << std::endl;
    std::cout << "Got Unity Player base at: 0x" << UP_image_base << std::endl;
    
    std::thread(Engine::Camera_Handler).detach();
    std::thread(Engine::Player_Handler).detach();
    std::thread(Engine::Bone_Handler).detach();
    std::thread(Engine::Projectile_Handler).detach();
    std::thread(Engine::Weapon_Handler).detach();

    Hijack::Hijack();

    Render::Init();

    Render::Draw();

	return 0;
}