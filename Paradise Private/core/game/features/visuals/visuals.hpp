#include <dependencies/framework/imgui.h>

namespace jrt
{
	class visuals_c
	{
	public:
		auto ActorLoop( ) -> void;
		auto TempThread() -> void;
	};
	inline visuals_c visuals;
}