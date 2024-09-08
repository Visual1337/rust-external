#pragma once
#include <cstdint>
#include "driver.h"

struct BaseEntityConvar
{
	auto get_base_entity() -> uintptr_t // System_Collections_Generic_List_BaseGameMode__c*
	{
		return IO::chain_read<uintptr_t>(game_image_address, { 60666648, 0xB8, 0x0, 0x10 }); // "Signature": "BaseEntity_c*"
	}
};

static BaseEntityConvar* BaseEntity = new BaseEntityConvar();