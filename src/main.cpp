#include <Windows.h>
#include "gui.h"
#include "features.h"
#include "globals.h"
#include <thread>

INT APIENTRY WinMain(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ PSTR arguments,
	_In_ INT cmd_show)
{
	Memory memory{ "csgo.exe" };
	globals::clientAddress = memory.GetModuleAddress("client.dll");

	std::thread(features::VisualsThread, memory).detach();

	// Create GUI
	gui::CreateHWindow("Menu", "Menu Class");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Destroy GUI
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}

	/*int fullScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int fullScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins
		{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}*/