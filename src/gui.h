#pragma once
#include <d3d11.h>

namespace gui
{
	// GUI Width & Height
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;
	extern int fullScreenWidth;
	extern int fullScreenHeight;

	// Program Exit Bool
	inline bool exit = true;

	// Winapi Window Variables
	inline HWND window = nullptr;
	inline WNDCLASSEXW windowClass = {};

	// Points for window movement
	inline POINTS position = {};

	// DirectX State Variables
	 inline ID3D11Device* device{ nullptr };
	 inline ID3D11DeviceContext* device_context{ nullptr };
	 inline IDXGISwapChain* swap_chain{ nullptr };
	 inline ID3D11RenderTargetView* render_target_view{ nullptr };
	 inline DXGI_SWAP_CHAIN_DESC* swap_chain_desc{ nullptr };
	 inline ID3D11Texture2D* back_buffer{ nullptr };
	 inline D3D_FEATURE_LEVEL level{};
	 inline constexpr D3D_FEATURE_LEVEL featureLevels[2]
	 {
		 D3D_FEATURE_LEVEL_11_0,
		 D3D_FEATURE_LEVEL_10_0
	 };

	// Window Creation & Destruction
	void CreateHWindow(
		const char* windowName,
		const char* className) noexcept;
	void DestroyHWindow() noexcept;

	// Handle Movement of the window with the mouse
	void HandleMouseMove(LPARAM longParameter) noexcept;

	// Device Creation & Destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// ImGui Creation & Destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	// Rendering
	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}