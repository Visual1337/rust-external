#pragma once
#include "driver.h"
#include "Settings.h"
#include <string>
#include <iostream>

class held_ent
{
public:
    auto get_created_projectiles(void (*each)(std::uintptr_t)) -> void
    {
        const auto list = IO::read<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(this) + 0x398);
        const auto key = IO::read<std::uintptr_t>(list + 0x10);
        const auto size = IO::read<int>(list + 0x18);

        for (int i = 0; i < size; ++i)
        {
            const auto projectile = IO::read<std::uintptr_t>(key + 0x20 + (i * 0x8));

            if (!projectile)
            {
                continue;
            }

            each(projectile);
        }
    }

    auto instant_eoka() -> void
    {
        IO::write<float>(reinterpret_cast<uintptr_t>(this) + 0x3A0, 100.f);//successFraction
        IO::write<bool>(reinterpret_cast<uintptr_t>(this) + 0x3B0, true);//_didSparkThisFrame
        IO::write<int>(reinterpret_cast<uintptr_t>(this) + 0x3B4, 1);//strikes
    }
};

#pragma comment(lib, "Winmm.lib")

class _BaseProjectile_
{
public:
    uintptr_t GetHeldItem(uintptr_t LocalPlayer)
    {
        auto base_player = LocalPlayer;
        
        const auto active_item_id = IO::read<uintptr_t>(base_player + oActiveUID);
        
        const auto player_inventory = IO::read<uintptr_t>(base_player + oPlayerInventory);
        
        const auto inventory_belt = IO::read<uintptr_t>(player_inventory + oContainerBelt);
        
        const auto belt_contents_list = IO::read<uintptr_t>(inventory_belt + oBeltContentList);
        
        const auto belt_contents_sz = IO::read<uintptr_t>(belt_contents_list + 0x14);
        
        const auto belt_contents = IO::read<uintptr_t>(belt_contents_list + 0x10);
       
        for (auto i = 0; i <= 6; i++)
        {
            if (belt_contents != 0)
            {
                const auto current_item = IO::read<uintptr_t>(belt_contents + (0x20 + (i * 8)));
                
                if (!current_item) continue;
                if (active_item_id == IO::read<uintptr_t>(current_item + 0x20)) return current_item;
            }
        }
    }

    auto fat_bullet(held_ent* heldItem)
    {
        auto set_thickness = [](std::uintptr_t each) -> void
            {
                IO::write<float>(each + 0x2C, 1.0f);
                auto mod = IO::read<uintptr_t>(each + 0xE8);
                IO::write<float>(mod + 0x30, 0.1f);
                IO::write<float>(mod + 0x38, 0.1f);

                auto HitTest = IO::read<uintptr_t>(each + 0x1A0);
                auto didHit = IO::read_string1(IO::read<uintptr_t>(HitTest + 0xC0) + 0x14);
                std::cout << didHit << std::endl;
                if (didHit == "Flesh" || didHit == "Metal")
                    PlaySoundA("C:\\hit.wav", NULL, SND_FILENAME);
            };

        heldItem->get_created_projectiles(set_thickness);
    }

    std::string get_item_name(uintptr_t base_projectile)
    {
        auto item_info = IO::read<uintptr_t>(base_projectile + 0x18); //Item->info

        auto display_name = IO::read<uintptr_t>(item_info + 0x20); //ItemDef->shortname

        auto wide_name = IO::read_wstring(display_name + 0x14, sizeof(display_name));

        std::string weapon_name = std::string(wide_name.begin(), wide_name.end());

        return weapon_name;
    }

    bool is_weapon(uintptr_t helditem)
    {
        auto ItemName = get_item_name(helditem);
        if (ItemName.find("rifle") != std::string::npos) return true;
        if (ItemName.find("pistol") != std::string::npos) return true;
        if (ItemName.find("bow.hunting") != std::string::npos) return true;
        if (ItemName.find("bow") != std::string::npos) return true;
        if (ItemName.find("lmg") != std::string::npos) return true;
        if (ItemName.find("shotgun") != std::string::npos) return true;
        if (ItemName.find("smg") != std::string::npos) return true;
        else return false;
    }

    bool is_item(uintptr_t helditem, const char* item_name)
    {
        auto ItemName = get_item_name(helditem);
        if (ItemName.find(item_name) != std::string::npos) return true;
        else return false;
    }

    bool is_melee(uintptr_t helditem)
    {
        auto ItemName = get_item_name(helditem);
        if (ItemName.find("rock") != std::string::npos) return true;
        if (ItemName.find("hatchet") != std::string::npos) return true;
        if (ItemName.find("stone.pickaxe") != std::string::npos) return true;
        if (ItemName.find("stonehatchet") != std::string::npos) return true;
        if (ItemName.find("pickaxe") != std::string::npos) return true;
        if (ItemName.find("hammer.salvage") != std::string::npos) return true;
        if (ItemName.find("icepick.salvag") != std::string::npos) return true;
        if (ItemName.find("axe.salvaged") != std::string::npos) return true;
        if (ItemName.find("pitchfork") != std::string::npos) return true;
        if (ItemName.find("mace") != std::string::npos) return true;
        if (ItemName.find("spear.stone") != std::string::npos) return true;
        if (ItemName.find("spear.wooden") != std::string::npos) return true;
        if (ItemName.find("machete") != std::string::npos) return true;
        if (ItemName.find("bone.club") != std::string::npos) return true;
        if (ItemName.find("paddle") != std::string::npos) return true;
        if (ItemName.find("salvaged.sword") != std::string::npos) return true;
        if (ItemName.find("salvaged.cleav") != std::string::npos) return true;
        if (ItemName.find("knife.combat") != std::string::npos) return true;
        if (ItemName.find("knife.butcher") != std::string::npos) return true;
        if (ItemName.find("jackhammer") != std::string::npos) return true;
        if (ItemName.find("chainsaw") != std::string::npos) return true;
        if (ItemName.find("knife.bone") != std::string::npos) return true;
        if (ItemName.find("torch") != std::string::npos) return true;
        if (ItemName.find("hammer") != std::string::npos) return true;
        if (ItemName.find("sickle") != std::string::npos) return true;
        else return false;
    }
}; static _BaseProjectile_* BaseProjectile = new _BaseProjectile_();