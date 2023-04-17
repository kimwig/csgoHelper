#include "gui.h"
#include "globals.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_dx11.h"
#include "../external/imgui/imgui_impl_win32.h"
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window, 
	UINT message, 
	WPARAM wideParameter, 
	LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter);

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU)
			return 0;
	}break;

	case WM_CLOSE: {
		DestroyWindow(window);
		gui::exit = false;
	}return 0;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter);
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			gui::HandleMouseMove(longParameter);
		}
	}return 0;

	default:
		break;
	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::HandleMouseMove(LPARAM longParameter) noexcept
{
	const auto points = MAKEPOINTS(longParameter);
	auto rect = ::RECT{};

	GetWindowRect(gui::window, &rect);

	rect.left += points.x - gui::position.x;
	rect.top += points.y - gui::position.y;

	if (gui::position.x >= 0 &&
		gui::position.x <= gui::WIDTH &&
		gui::position.y >= 0 && gui::position.y <= 19)
		SetWindowPos(
			gui::window,
			HWND_TOPMOST,
			rect.left,
			rect.top,
			0, 0,
			SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
		);
}

void gui::CreateHWindow(
	const char* windowName,
	const char* className) noexcept
{
	const int windowNameLength = static_cast<int>(strlen(windowName) + 1);
	const int classNameLength = static_cast<int>(strlen(className) + 1);

	std::wstring wideWindowName(windowNameLength, L'\0');
	std::wstring wideClassName(classNameLength, L'\0');

	MultiByteToWideChar(CP_UTF8, 0, windowName, windowNameLength, wideWindowName.data(), windowNameLength);
	MultiByteToWideChar(CP_UTF8, 0, className, classNameLength, wideClassName.data(), classNameLength);

	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = wideClassName.c_str();
	windowClass.hIconSm = nullptr;

	RegisterClassExW(&windowClass);

	window = CreateWindowExW(
		0,
		wideClassName.c_str(),
		wideWindowName.c_str(),
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);
	if (window == nullptr)
	{
		return;
	}

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = WIDTH;
	swapChainDesc.BufferDesc.Height = HEIGHT;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60U;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1U;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1U;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2U;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	if (FAILED(hr))
		return false;

	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
		back_buffer = nullptr;
	}
	else
	{
		return false;
	}

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX11_InvalidateDeviceObjects();

	if (!CreateDevice())
	{
		return;
	}

	DestroyDevice();

	ImGui_ImplDX11_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (device_context)
	{
		device_context->Release();
		device_context = nullptr;
	}

	if (swap_chain)
	{
		swap_chain->Release();
		swap_chain = nullptr;
	}

	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ::ImGui::GetIO();
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
		if (message.message == WM_QUIT)
			break;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device_context->OMSetDepthStencilState(nullptr, 0);
	device_context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	device_context->RSSetState(nullptr);

	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
	device_context->ClearRenderTargetView(render_target_view, clear_color);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	const auto result = swap_chain->Present(1U, 0U);

	if (result == DXGI_ERROR_DEVICE_RESET || result == DXGI_ERROR_DEVICE_REMOVED)
		ResetDevice();
}

void gui::Render() noexcept
{
	ImGui::SetWindowSize({ 0,0 });
	ImGui::SetNextWindowSize({ WIDTH,HEIGHT });
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::Begin(
		"Feature Menu",
		&exit,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	ImGui::Checkbox("Radar", &globals::radar);

	ImGui::Checkbox("Glow", &globals::glow);
	ImGui::ColorEdit4("Glow Color", globals::glowColor);

	ImGui::End();
}