#include "interface.hpp"

auto jrt::screen_c::GetScreenSize( ) -> bool
{
	this->fWidth = GetSystemMetrics( SM_CXSCREEN );
	this->fHeight = GetSystemMetrics( SM_CYSCREEN );
	return ( this->fWidth != 0 && this->fHeight != 0 );
}