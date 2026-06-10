#pragma once

#include "obse64_common/Types.h"

namespace RE
{
	namespace RefProtection
	{
		// Local RVAs confirmed against Oblivion Remastered 1.512.105.
		// TODO(RE): Convert these to address-library IDs if required upstream.
		constexpr uintptr_t kDeathGuardHookRVA = 0x065B9538;
		constexpr uintptr_t kDeathGuardDeathPathRVA = 0x065B99D3;
		constexpr uintptr_t kDeathGuardEssentialFallthroughRVA = 0x065B9548;
		constexpr uintptr_t kNativeIsEssentialRVA = 0x065B0D10;
		constexpr u32 kDeathGuardPatchSize = 0x10;

		constexpr uintptr_t kCrosshairIconHookRVA = 0x06580616;
		constexpr uintptr_t kCrosshairIconNormalReturnRVA = 0x06580705;
		constexpr uintptr_t kCrosshairIconEssentialReturnRVA = 0x0658062F;
		constexpr u32 kCrosshairIconPatchSize = 0x19;

		constexpr u32 kActorFormIDOffset = 0x10;
	}
}
