#include <core/core.hpp>

 auto main() -> int
 {
	if (!jrt::core.Initialize())
	{
		MessageBoxA(GetForegroundWindow(), _("failed to init contact support."), _("Paradise"), MB_OK | MB_ICONERROR);
		std::cin.get();
	}
}