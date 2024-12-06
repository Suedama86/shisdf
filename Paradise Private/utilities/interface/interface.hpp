#include <Windows.h>
#include <iostream>
#include <dependencies/framework/imgui.h>

namespace jrt
{
	class interface_c
	{
	public:
		static auto Print( const char* cText ) -> void
		{
			std::cout << cText << '\n';
		}
		template <typename type>

		static auto Print( const char* cText, type tType ) -> void
		{
			std::cout << cText << tType << '\n';
		}
	};
	inline interface_c Interface;

	class screen_c
	{
	public:
		float fWidth, fHeight;
		auto GetScreenSize( ) -> bool;
	};
	inline screen_c screen;

	class fonts_c
	{
	public:

		ImFont* Menu;
		ImFont* Esp;
		ImFont* Icons;
	};
	inline fonts_c fonts;
}