#include <time.h>

#include "framework.h"

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework* p{ reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)) };
	return p ? p->handle_message(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prev_instance, _In_ LPSTR cmd_line, _In_ int cmd_show)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	window::initialize(instance, cmd_show);

	Framework framework(window::getHwnd());
	SetWindowLongPtrW(window::getHwnd(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&framework));
	return framework.run();
}
