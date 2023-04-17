#include "features.h"
#include "globals.h"
#include "gui.h"
#include <thread>

void features::VisualsThread(const Memory& memory) noexcept
{
	while (gui::exit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		const auto localPlayer = memory.Read<std::uintptr_t>(globals::clientAddress + offsets::dwLocalPlayer);

		if (!localPlayer)
			continue;

		const auto glowManager = memory.Read<std::uintptr_t>(globals::clientAddress + offsets::dwGlowObjectManager);

		if (!glowManager)
			continue;

		const auto localTeam = memory.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto player = memory.Read<std::uintptr_t>(globals::clientAddress + offsets::dwEntityList + i * 0x10);

			if (!player)
				continue;

			const auto team = memory.Read<std::int32_t>(player + offsets::m_iTeamNum);

			if (team == localTeam)
				continue;

			const auto lifeState = memory.Read<std::int32_t>(player + offsets::m_lifeState);

			if (lifeState != 0)
				continue;

			if (globals::glow)
			{
				const auto glowIndex = memory.Read<std::int32_t>(player + offsets::m_iGlowIndex);

				memory.Write(glowManager + (glowIndex * 0x38) + 0x8, globals::glowColor[0]);
				memory.Write(glowManager + (glowIndex * 0x38) + 0xC, globals::glowColor[1]);
				memory.Write(glowManager + (glowIndex * 0x38) + 0x10, globals::glowColor[2]);
				memory.Write(glowManager + (glowIndex * 0x38) + 0x14, globals::glowColor[3]);

				memory.Write(glowManager + (glowIndex * 0x38) + 0x28, true);
				memory.Write(glowManager + (glowIndex * 0x38) + 0x29, false);
			}

			if (globals::radar)
			{
				memory.Write(player + offsets::m_bSpotted, true);
			}
		}
	}
}