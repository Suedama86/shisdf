#include "core.hpp"

auto jrt::core_c::Initialize() -> bool
{
	if (!Driver::Init())
	{
		printf(_("\n [-] Driver not Loaded!\n"));
	}
	else
	{
		printf(_("\n [+] Waiting for Fortnite.\n"));
	}

	if (!jrt::screen.GetScreenSize())
	{
		jrt::Interface.Print(_("[+] Failed to get Screen Size.\n"));
		return false;
	}

	while (hwnd == NULL)
	{
		hwnd = FindWindowA(0, _("Fortnite  "));
		processID = Driver::FindProcess(_(L"FortniteClient-Win64-Shipping.exe"));
		Sleep(1);
	}

	system(_("cls"));

	EAC = true;

	if (!Driver::CR3())
	{
		std::cout << _("[+] No CR3 Found!") << std::endl;
		Sleep(-1);
	}

	Base = Driver::GetBase();

    std::thread([]() { jrt::cache.Data(); }).detach();
	std::thread([]() { jrt::cache.Entities(); }).detach();
	std::thread([]() { jrt::combat.CombatThread(); }).detach();


	if (!jrt::render.Initialize())
	{
		jrt::Interface.Print(_("[+] Failed to Initialize DX11.\n"));
		return false;
	}
	else
		jrt::render.RenderThread();


	return false;


	std::cin.get();
}