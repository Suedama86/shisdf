#include <Windows.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <utilities/obfuscation/xor.hpp>
#include <core/game/sdk.hpp>
#include <core/game/cache/cache.hpp>
#include <core/render/render.hpp>
#include <core/game/features/combat/combat.hpp>
#include <core/game/features/visuals/visuals.hpp>

namespace jrt
{
	class core_c
	{
	public:
		auto Initialize( ) -> bool;
	};
	inline core_c core;
}