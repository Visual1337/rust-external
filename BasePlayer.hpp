#pragma once
#include <Windows.h>
#include "driver.h"
#include "Settings.h"
#include "Math.h"

namespace Enums
{
	enum PlayerFlags
	{
		Unused1 = 1,
		Unused2 = 2,
		IsAdmin = 4,
		ReceivingSnapshot = 8,
		Sleeping = 16,
		Spectating = 32,
		Wounded = 64,
		IsDeveloper = 128,
		Connected = 256,
		ThirdPersonViewmode = 1024,
		EyesViewmode = 2048,
		ChatMute = 4096,
		NoSprint = 8192,
		Aiming = 16384,
		DisplaySash = 32768,
		Relaxed = 65536,
		SafeZone = 131072,
		ServerFall = 262144,
		Incapacitated = 524288,
		Workbench1 = 1048576,
		Workbench2 = 2097152,
		Workbench3 = 4194304,
		VoiceRangeBoost = 8388608,
		ModifyClan = 16777216,
		LoadingAfterTransfer = 33554432,
		NoRespawnZone = 67108864,
		IsInTutorial = 134217728
	};
}

ViewMatrix CurrentMatrix;

Vector2 WorldToScreen(const Vector3& position)
{
	Vector3 transVec = Vector3(CurrentMatrix._14, CurrentMatrix._24, CurrentMatrix._34);
	Vector3 rightVec = Vector3(CurrentMatrix._11, CurrentMatrix._21, CurrentMatrix._31);
	Vector3 upVec = Vector3(CurrentMatrix._12, CurrentMatrix._22, CurrentMatrix._32);

	float w = Vector3::Dot(transVec, position) + CurrentMatrix._44;

	if (w < 0.098f)
		return Vector2(0, 0);
	
	float x = Vector3::Dot(rightVec, position) + CurrentMatrix._41;
	float y = Vector3::Dot(upVec, position) + CurrentMatrix._42;

	return Vector2((GetSystemMetrics(SM_CXSCREEN) / 2) * (1.f + x / w), (GetSystemMetrics(SM_CYSCREEN) / 2) * (1.f - y / w));
}

bool W2S(const Vector3& position, Vector2& output)
{
	Vector3 transVec = Vector3(CurrentMatrix._14, CurrentMatrix._24, CurrentMatrix._34);
	Vector3 rightVec = Vector3(CurrentMatrix._11, CurrentMatrix._21, CurrentMatrix._31);
	Vector3 upVec = Vector3(CurrentMatrix._12, CurrentMatrix._22, CurrentMatrix._32);

	float w = Vector3::Dot(transVec, position) + CurrentMatrix._44;

	if (w < 0.098f)
		output = Vector2(0,0);

	float x = Vector3::Dot(rightVec, position) + CurrentMatrix._41;
	float y = Vector3::Dot(upVec, position) + CurrentMatrix._42;
	output = Vector2((GetSystemMetrics(SM_CXSCREEN) / 2) * (1.f + x / w), (GetSystemMetrics(SM_CYSCREEN) / 2) * (1.f - y / w));
	return true;
}

class player_model
{
public:
	auto is_npc() -> bool
	{
		return IO::read<bool>(reinterpret_cast<uintptr_t>(this) + 0x2D0);
	}

	auto get_position() -> Vector3
	{
		return IO::read<Vector3>(reinterpret_cast<uintptr_t>(this) + 0x1C0);
	}

	auto get_skintype()
	{
		return IO::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x16C);
	}

	auto is_visible() -> bool
	{
		return IO::read<bool>(reinterpret_cast<uintptr_t>(this) + 0x22C);
	}

	auto is_local_player() -> bool
	{
		return IO::read<bool>(reinterpret_cast<uintptr_t>(this) + 0x241);
	}

	auto look_at(Vector2 angles) -> void
	{
		angles.normalize();
		IO::write<Vector4>(reinterpret_cast<uintptr_t>(this) + 0x198, to_quat({ angles.x,angles.y, 0 }));
	}
};

class player_eyes
{
public:
	auto get_eye_offset() -> Vector3
	{
		return IO::read<Vector3>(reinterpret_cast<uintptr_t>(this) + 0x0);
	}

	auto look_at(Vector2 angles) -> void
	{
		angles.normalize();
		IO::write<Vector4>(reinterpret_cast<uintptr_t>(this) + 0x44, to_quat({ angles.x,angles.y, 0 }));
	}
};

class player_input
{
public:
	
};

class model_state
{
public:
	auto has_flag(int flag) -> bool
	{
		return IO::read<int>(reinterpret_cast<uintptr_t>(this) + 0x20) & flag;
	}
};

class main_camera
{
public:
	static main_camera* get_instance()
	{
		return IO::chain_read<main_camera*>(game_image_address, { 61469048, 0xB8, 0x0, 0x10 });
	}

	static uintptr_t get_instance1()
	{
		return IO::chain_read<uintptr_t>(game_image_address, { 61469048, 0xB8, 0x0, 0x10 });
	}

	Vector3 position()
	{
		return IO::read<Vector3>(get_instance1() + 0x42C);
	}
};

class _Base_Local_Player_
{
public:
	static auto get_instance() -> uintptr_t
	{
		return IO::chain_read<uintptr_t>(game_image_address, { m_localplayer_c, 0xB8, 0x0, 0x10, 0x28 });
	}

	static auto get_object_list() -> uintptr_t
	{
		return IO::chain_read<uintptr_t>(game_image_address, { m_localplayer_c, 0xB8, 0x0, 0x10, 0x28, 0x18 });
	}

	auto get_display_name() -> std::string
	{
		return IO::read_string1(get_instance() + 0x950);
	}

	auto get_player_model() -> player_model*
	{
		return IO::read<player_model*>(get_instance() + oPlayerModel);
	}

	auto get_player_model1() -> uintptr_t
	{
		return IO::read<uintptr_t>(get_instance() + oPlayerModel);
	}

	static auto is_dead() -> bool
	{
		return IO::read<bool>(get_instance() + 0x26C);
	}

	static auto set_player_flags(Enums::PlayerFlags player_flag) -> VOID
	{
		IO::write(get_instance() + 0x8F0, player_flag);
	}

	static auto is_mounted() -> uintptr_t
	{
		auto idk = IO::read<uintptr_t>(get_instance() + 0x828);
		return IO::write<bool>(idk + 0x3A8, false);
	}

	auto get_player_input() -> player_input*
	{
		return IO::read<player_input*>(reinterpret_cast<uintptr_t>(this) + 0x6B0);
	}

	static auto get_eyes() -> player_eyes*
	{
		return IO::read<player_eyes*>(get_instance() + 0x8F8);
	}
};

struct transform_access_read_only_t
{
	uint64_t transform_data{};
};

struct transform_data_t
{
	uint64_t transform_array{};
	uint64_t transform_indices{};
};

struct matrix34_t
{
	Vector4 vec0{};
	Vector4 vec1{};
	Vector4 vec2{};
};

class transform_c
{
public:
	Vector3 get_position_injected()
	{
		uintptr_t transform = IO::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x10);

		__m128 result{};

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		transform_access_read_only_t transform_access_read_only = IO::read<transform_access_read_only_t>(transform + 0x38);
		unsigned int index = IO::read<unsigned int>(transform + 0x40);
		transform_data_t transform_data = IO::read<transform_data_t>(transform_access_read_only.transform_data + 0x18);

		if (transform_data.transform_array && transform_data.transform_indices)
		{
			result = IO::read<__m128>(transform_data.transform_array + 0x30 * index);
			int transform_index = IO::read<int>(transform_data.transform_indices + 0x4 * index);
			int safe = 0;
			while (transform_index >= 0 && safe++ < 200)
			{
				matrix34_t matrix = IO::read<matrix34_t>(transform_data.transform_array + 0x30 * transform_index);

				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x00));	// xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x55));	// yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x8E));	// zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xDB));	// wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xAA));	// zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x71));	// yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix.vec2), result);

				result = _mm_add_ps(_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix.vec0));

				transform_index = IO::read<int>(transform_data.transform_indices + 0x4 * transform_index);
			}
		}

		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
};

class _BasePlayer_
{
public:
	static auto get_instance() -> uintptr_t
	{
		return IO::chain_read<uintptr_t>(game_image_address, { m_localplayer_c, 0xB8, 0x0, 0x10, 0x28});
	}

	static auto get_object_list() -> uintptr_t
	{
		return IO::chain_read<uintptr_t>(game_image_address, { m_localplayer_c, 0xB8, 0x0, 0x10, 0x28, 0x18 });
	}

	Vector3 get_bone(int bone_id)
	{
		uintptr_t player_model = IO::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0xB8);
		
		uintptr_t bone_transforms = IO::read<uintptr_t>(player_model + 0x48);
		
		transform_c* transform = IO::read<transform_c*>(bone_transforms + (0x20 + (bone_id * 0x8)));
		return transform->get_position_injected();
	}

	auto get_player_input() -> player_input*
	{
		return IO::read<player_input*>(reinterpret_cast<uintptr_t>(this) + 0x6B0);
	}

	auto get_model_state() -> model_state*
	{
		return IO::read<model_state*>(reinterpret_cast<uintptr_t>(this) + 0x38);
	}

	auto get_display_name() -> std::string
	{
		return IO::read_wstr(IO::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x950) + 0x14);
	}

	auto get_player_model() -> player_model*
	{
		return IO::read<player_model*>(reinterpret_cast<uintptr_t>(this) + oPlayerModel);
	}

	auto get_player_eye() -> player_eyes*
	{
		return IO::read<player_eyes*>(reinterpret_cast<uintptr_t>(this) + 0x8F8);
	}

	auto is_dead() -> bool
	{
		return IO::read<bool>(reinterpret_cast<bool>(this) + 0x26C);
	}

	static auto set_player_flags(Enums::PlayerFlags player_flag) -> VOID
	{
		IO::write(get_instance() + 0x8F0, player_flag);
	}

	auto is_mounted() -> uintptr_t
	{
		auto idk = IO::read<uintptr_t>(get_instance() + 0x828);
		return IO::write<bool>(idk + 0x3A8, false);
	}
};

struct CacheList
{
	_BasePlayer_* Entity;
	_Base_Local_Player_* LocalPlayer;
};

struct BoneList
{
	Vector2 
		head, 
		spine, 
		bottom,
		l_upperarm, 
		l_forearm, 
		l_hand, 
		r_upperarm, 
		r_forearm, 
		r_hand, 
		pelvis, 
		pelvis1, 
		l_hip, 
		l_knee, 
		l_foot, 
		r_hip, 
		r_knee, 
		r_foot, 
		l_toe, 
		r_toe;
};

std::vector<CacheList> EntCacheList;
std::vector<BoneList> EntBoneList;

static _BasePlayer_* BasePlayer = new _BasePlayer_();
static _Base_Local_Player_* BaseLocalPlayer = new _Base_Local_Player_();