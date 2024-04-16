#pragma once

#include <d3d11.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <wrl/client.h>
#include <Keyboard.h>
#include <Mouse.h>

#include <dxgi.h>

#include "Graphics/Graphics.h"

#include "misc.h"
#include "highResolutionTimer.h"


#ifdef USE_IMGUI
#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../External/ImGui/imgui_impl_dx11.h"
#include "../External/ImGui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#include "sceneManager.h"

#include "game.h"
#include "title.h"

#include "Graphics/geometricPrimitive.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/FullScreenQuad.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/staticMesh.h"
#include "Graphics/TextureManager.h"

#include "Graphics/ShaderToy.h"
#include "Graphics/DebugRenderer.h"
#include "Library/Camera.h"
#include "Graphics/SpriteRenderer.h"
#include "GameObject.h"
#include "./Graphics/Particle.h"
#include "./Library/Library.h"
#include "Graphics/Effect.h"

#include <dxgidebug.h>


class Framework {
public:
	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			uninitialize();
			return 0;
		}

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculateFrameStats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 0
		BOOL fullscreen = window::isFullScreen();
		Graphics::instance().getSwapChain()->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			Graphics::instance().getSwapChain()->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

		auto& input = InputManager::instance();

		input.m_.wheelDelta_ = 0;
		input.m_.wheel_ = 0;

		// マウスホイール用
		static int nWheelFraction = 0;	// 回転量の端数

#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;

		case WM_SIZE:
		{
			//RECT clientRect = {};
			//GetClientRect(window::getHwnd(), &clientRect);
			//window::onSizeChanged(static_cast<UINT64>(clientRect.right - clientRect.left), clientRect.bottom - clientRect.top);
			break;
		}

			//	keyboard
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			nWheelFraction = 0;
			break;

		case WM_SYSKEYDOWN:
			if (wparam == VK_RETURN && (lparam & 0x60000000) == 0x20000000) {

			}
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			break;

		case WM_KEYDOWN:
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			break;

			//	mouse
		case WM_MOUSEMOVE:
			input.mouse_->ProcessMessage(msg, wparam, lparam);
			break;

		case WM_MOUSEWHEEL:
		{

			// 画面外なら動作しない
			if (input::getCursorPos().x < 0 || input::getCursorPos().x >= window::getWidth() ||
				input::getCursorPos().y < 0 || input::getCursorPos().y >= window::getHeight())
				break;

			int zDelta = GET_WHEEL_DELTA_WPARAM(wparam);	// 回転量

			// 前回の端数を追加
			zDelta += nWheelFraction;

			// ノッチ数を求める
			int nNotch = zDelta / WHEEL_DELTA;

			// 端数を保存する
			nWheelFraction = zDelta % WHEEL_DELTA;

			if (nNotch == 0)
				break;

			float rate = nNotch > 0 ? 1.04f : 0.96f;

			if (nNotch > 0)
			{
				Camera::instance().zoom(1);
				input.m_.wheel_ = 1;
			}

			else
			{
				Camera::instance().zoom(-1);
				input.m_.wheel_ = -1;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			InputManager::instance().mouse_->ProcessMessage(msg, wparam, lparam);
			break;

		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsedTime_{ 0.0f };
	void calculateFrameStats() {

		if (++frames, (tictoc.time_stamp() - elapsedTime_) >= 1.0f) {

			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(window::getHwnd(), outs.str().c_str());

			frames = 0;
			elapsedTime_ += 1.0f;
		}
	}

#ifdef USE_IMGUI
	void setImGuiStyle();
	void ImGuiStyleEditor();
#endif

	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[8];

	std::unique_ptr<ShaderToy> shaderToy_;

	std::unique_ptr<Particle> particle_;

	std::unique_ptr<Effect> effect_;
};