#pragma once

#include <Windows.h>
#include "driver.h"
#include "BaseProjectile.hpp"
#include "BasePlayer.hpp"
#include "BaseEntity.hpp"
#include <mutex> 
#include <chrono>

float originalx1;
float originalx2;

float originaly1;
float originaly2;

std::vector<_BasePlayer_> player_list;

std::mutex cacheMutex;

namespace Engine
{
	VOID Chams_Handler()
	{

	}

	VOID Camera_Handler()
	{
		for (;;)
		{
			CurrentMatrix = IO::chain_read<ViewMatrix>(game_image_address, { 61469048 , 0xB8, 0x0, 0x10, 0x2E4 }); //MainCamera_C*
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}

	std::vector<CacheList> TempCacheList;
	std::vector<BoneList> TempBoneList;

	VOID Bone_Handler()
	{
		for (;;)
		{
			TempBoneList.clear();

			auto oPlayerList = IO::chain_read<DWORD64>(game_image_address, { 61582856 , 0xB8, 0x20 }); //BasePlayer_C*
			auto ClientEntities_Value = IO::read<UINT64>(oPlayerList + 0x28);
			auto EntityCount = IO::read<UINT64>(ClientEntities_Value + 0x10);
			UINT64 EntityBuffer = IO::read<UINT64>(ClientEntities_Value + 0x18);

			for (int i = 1; i < EntityCount; i++)
			{
				UINT64 Entity = IO::read<UINT64>(EntityBuffer + 0x20 + (i * 0x8));

				_BasePlayer_* DrawEntity = (_BasePlayer_*)Entity;

				if (DrawEntity->get_player_model()->is_npc())
					continue;

				if (DrawEntity->is_dead())
					continue;

				if (DrawEntity->get_player_model()->get_position() == Vector3(0, 0, 0))
					continue;

				if (DrawEntity->get_player_model()->is_local_player())
					continue;

				if (DrawEntity->get_model_state()->has_flag(16))
					continue;

				Vector3 EntPos = DrawEntity->get_player_model()->get_position();

				Vector2 EntityPos = WorldToScreen(EntPos);

				if (EntityPos.x == 0 || EntityPos.y == 0)
					continue;

				BoneList List{ };

				auto head_b = DrawEntity->get_bone(head);
				auto spine4_b = DrawEntity->get_bone(spine4);
				auto l_upperarm_b = DrawEntity->get_bone(l_upperarm);
				auto l_forearm_b = DrawEntity->get_bone(l_forearm);
				auto l_hand_b = DrawEntity->get_bone(l_hand);
				auto r_upperarm_b = DrawEntity->get_bone(r_upperarm);
				auto r_forearm_b = DrawEntity->get_bone(r_forearm);
				auto r_hand_b = DrawEntity->get_bone(r_hand);
				auto pelvis1_b = DrawEntity->get_bone(7);
				auto l_hip_b = DrawEntity->get_bone(l_hip);
				auto l_knee_b = DrawEntity->get_bone(l_knee);
				auto l_foot_b = DrawEntity->get_bone(l_foot);
				auto r_hip_b = DrawEntity->get_bone(r_hip);
				auto r_knee_b = DrawEntity->get_bone(r_knee);
				auto r_foot_b = DrawEntity->get_bone(r_foot);
				auto r_toe_b = DrawEntity->get_bone(r_toe);
				auto l_toe_b = DrawEntity->get_bone(l_toe);
				auto bottom_b = DrawEntity->get_player_model()->get_position();

				List.head = WorldToScreen(head_b);
				List.bottom = WorldToScreen(bottom_b);
				List.spine = WorldToScreen(spine4_b);
				List.l_upperarm = WorldToScreen(l_upperarm_b);
				List.l_forearm = WorldToScreen(l_forearm_b);
				List.l_hand = WorldToScreen(l_hand_b);
				List.r_upperarm = WorldToScreen(r_upperarm_b);
				List.r_forearm = WorldToScreen(r_forearm_b);
				List.r_hand = WorldToScreen(r_hand_b);
				List.pelvis = WorldToScreen(pelvis1_b);
				List.l_hip = WorldToScreen(l_hip_b);
				List.l_knee = WorldToScreen(l_knee_b);
				List.l_foot = WorldToScreen(l_foot_b);
				List.r_hip = WorldToScreen(r_hip_b);
				List.r_knee = WorldToScreen(r_knee_b);
				List.r_toe = WorldToScreen(r_toe_b);
				List.l_toe = WorldToScreen(l_toe_b);
				List.r_foot = WorldToScreen(r_foot_b);

				TempBoneList.push_back(List);
			}

			EntBoneList.clear();
			EntBoneList = TempBoneList;

			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}

	VOID Player_Handler()
	{
		for (;;)
		{
			TempCacheList.clear();
			
			auto oPlayerList = IO::chain_read<DWORD64>(game_image_address, { 61582856 , 0xB8, 0x20}); //BasePlayer_C*
			auto ClientEntities_Value = IO::read<UINT64>(oPlayerList + 0x28);
			auto EntityCount = IO::read<UINT64>(ClientEntities_Value + 0x10);
			UINT64 EntityBuffer = IO::read<UINT64>(ClientEntities_Value + 0x18);

			for (int i = 1; i < EntityCount; i++)
			{
				UINT64 Entity = IO::read<UINT64>(EntityBuffer + 0x20 + (i * 0x8));
				_BasePlayer_* DrawEntity = (_BasePlayer_*)Entity;

				if (DrawEntity->get_player_model()->is_npc())
					continue;

				if (DrawEntity->is_dead())
					continue;

				if (DrawEntity->get_player_model()->get_position() == Vector3(0, 0, 0))
					continue;

				if (DrawEntity->get_player_model()->is_local_player())
					continue;

				if (DrawEntity->get_model_state()->has_flag(16))
					continue;

				/*if (!DrawEntity->get_player_model()->is_visible())
					continue;*/

				CacheList List{ };

				List.Entity = DrawEntity;
				List.LocalPlayer = BaseLocalPlayer;
				
				TempCacheList.push_back(List);
			}

			EntCacheList.clear();
			EntCacheList = TempCacheList;

			std::this_thread::sleep_for(std::chrono::nanoseconds(10));
		}
		
	}

	VOID Object_Handler()
	{
		/*auto BaseNetworkable = IO::chain_read<uintptr_t>(game_image_address, { 60670200 , 0xB8 });
		for (;;)
		{
			auto clientEntities = IO::read<uintptr_t>(BaseNetworkable + 0x0);
			auto EntityList = IO::read<uintptr_t>(clientEntities + 0x10);
			auto unk1 = IO::read<uintptr_t>(EntityList + 0x28);
			auto unk1_list = IO::read<uintptr_t>(EntityList + 0x18);
			auto unk1_size = IO::read<uintptr_t>(EntityList + 0x10);

			for (int i = 0; i < unk1_size; i++)
			{
				auto current_object = IO::read<uintptr_t>(unk1_list + (0x20 + (i * 8)));
				auto obj_class = IO::read<uintptr_t>(current_object + 0x30);
				auto obj_transform = IO::read<uintptr_t>(current_object + 0x8);
				auto obj_state = IO::read<uintptr_t>(current_object + 0x38);
				auto obj_pos = IO::read<Vector3>(current_object + 0xB0);

				std::cout << obj_pos.x << std::endl;
			}

			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}*/
	}

	VOID Projectile_Handler()
	{
		for (;;)
		{
			const auto current_item = BaseProjectile->GetHeldItem(BasePlayer->get_instance());
			auto base_projectile1 = IO::read<held_ent*>(current_item + oHeld);
			if (BaseProjectile->is_weapon(current_item))
			{
				BaseProjectile->fat_bullet(base_projectile1);
			}
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}

	VOID Weapon_Handler()
	{
		for (;;)
		{
			const auto current_item = BaseProjectile->GetHeldItem(BasePlayer->get_instance());
			
			auto base_projectile = IO::read<uintptr_t>(current_item + oHeld);
			auto base_projectile1 = IO::read<held_ent*>(current_item + oHeld);
			
			auto recoil_prop = IO::read<uintptr_t>(base_projectile + oRecoilProperties);
			
			auto newRecoilOverride = IO::read<uintptr_t>(recoil_prop + 0x80); //RecoilProperties newRecoilOverride;
			
			if (settings->NoRecoil) {
				if (BaseProjectile->is_weapon(current_item)) {

					IO::write<float>(base_projectile + 0x20C, 0.1f);
					IO::write<float>(base_projectile + 0x208, 0.1f);
					IO::write<float>(base_projectile + 0x210, 0.1f);

					if (newRecoilOverride) {
						IO::write<int>(newRecoilOverride + oRecoilYawMin, originalx1 * (settings->NoRecoilValue / 100));
						IO::write<int>(newRecoilOverride + oRecoilYawMax, originalx2 * (settings->NoRecoilValue / 100));
						IO::write<int>(newRecoilOverride + oRecoilPitchMin, originaly1 * (settings->NoRecoilValue / 100));
						IO::write<int>(newRecoilOverride + oRecoilPitchMax, originaly2 * (settings->NoRecoilValue / 100));
					}
					else
					{
						if (!newRecoilOverride) {
							IO::write<int>(recoil_prop + oRecoilYawMin, originalx1 * (settings->NoRecoilValue / 100));
							IO::write<int>(recoil_prop + oRecoilYawMax, originalx2 * (settings->NoRecoilValue / 100));
							IO::write<int>(recoil_prop + oRecoilPitchMin, originaly1 * (settings->NoRecoilValue / 100));
							IO::write<int>(recoil_prop + oRecoilPitchMax, originaly2 * (settings->NoRecoilValue / 100));
						}
					}
				}
			}

			if (BaseProjectile->is_item(current_item, "pistol.eoka")) {
				base_projectile1->instant_eoka();
			}
			
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}
}