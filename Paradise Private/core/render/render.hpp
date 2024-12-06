#include <Windows.h>
#include <iostream>

namespace jrt
{
	class render_c
	{
	public:
		auto Initialize( ) -> bool;
		auto RenderMenu( ) -> void;
		auto RenderThread( ) -> void;
		auto SetWindowToTarget() -> void;
	};
	inline render_c render;
}