#pragma once
#include <Windows.h>
#include "driver.h"
#include "Settings.h"
#include "Math.h"

class cBaseMiscEntity {
public:
	cBaseMiscEntity() {}

	cBaseMiscEntity(ULONG64 _ent, ULONG64 _trans, ULONG64 _obj) {
		this->ent = IO::read<ULONG64>(_ent + 0x28);
		this->trans = IO::read<ULONG64>(_trans + 0x38);
		this->name = IO::read_native(_obj + 0x60);

		if (this->name.find(("stone-ore")) != std::string::npos)
			this->name = ("STONE");
		else if (this->name.find(("metal-ore")) != std::string::npos)
			this->name = ("METAL");
		else if (this->name.find(("sulfur-ore")) != std::string::npos)
			this->name = ("SULFUR");
		else if (this->name.find(("stone-collectable")) != std::string::npos)
			this->name = ("STONE");
		else if (this->name.find(("metal-collectable")) != std::string::npos)
			this->name = ("METAL");
		else if (this->name.find(("sulfur-collectable")) != std::string::npos)
			this->name = ("SULFUR");
		else if (this->name.find(("wood-collectable")) != std::string::npos)
			this->name = ("WOOD");
		else if (this->name.find(("hemp-collectable")) != std::string::npos || this->name.find(("hemp.entity.prefab")) != std::string::npos)
			this->name = ("HEMP");
		else if (this->name.find(("small_stash_deployed")) != std::string::npos)
			this->name = ("STASH");
		else if (this->name.find(("loot_barrel_1.prefab")) != std::string::npos || this->name.find(("loot_barrel_2.prefab")) != std::string::npos || this->name.find(("loot-barrel-1.prefab")) != std::string::npos || this->name.find(("loot-barrel-2.prefab")) != std::string::npos)
			this->name = ("BARREL");
		else if (this->name.find(("oil_barrel.prefab")) != std::string::npos)
			this->name = ("OIL BARREL");
		else if (this->name.find(("crate_elite.prefab")) != std::string::npos)
			this->name = ("ELITE CRATE");
		else if (this->name.find(("crate_normal.prefab")) != std::string::npos)
			this->name = ("MILITARY CRATE");
		else if (this->name.find(("crate_normal_2_medical.prefab")) != std::string::npos)
			this->name = ("MEDICAL CRATE");
		else if (this->name.find(("crate_normal_2.prefab")) != std::string::npos)
			this->name = ("REGULAR CRATE");
		else if (this->name.find(("crate_normal_2_food.prefab")) != std::string::npos)
			this->name = ("FOOD CRATE");
		else if (this->name.find(("crate_tools.prefab")) != std::string::npos)
			this->name = ("TOOL CRATE");
		else if (this->name.find(("rowboat.prefab")) != std::string::npos)
			this->name = ("BOAT");
		else if (this->name.find(("rhib.prefab")) != std::string::npos)
			this->name = ("RHIB");
		else if (this->name.find(("kayak.prefab")) != std::string::npos)
			this->name = ("KAYAK");
		else if (this->name.find(("minicopter.entity.prefab")) != std::string::npos)
			this->name = ("MINICOPTER");
		else if (this->name.find(("bradleyapc.prefab")) != std::string::npos)
			this->name = ("BRADLEY");

		else
			this->name = ("Error");
	}

public:
	ULONG64 ent{};
	ULONG64 trans{};
	std::string name{};
};