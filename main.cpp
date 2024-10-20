#include "graphics.h"
#include "offsets.h"
#include "memory.h"
#include "math.h"
#include <iostream>





extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK winProc(HWND window, UINT message, WPARAM wprm, LPARAM lprm)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wprm, lprm))
	{
		return 0L;
	}

	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, wprm, lprm);
}

int Gamewidth{};
int Gameheight{};
WNDCLASSEXW wc{};
HWND window{nullptr};
ID3D11Device* device{ nullptr };
ID3D11DeviceContext* deviceContext{ nullptr };
IDXGISwapChain* swapChain{ nullptr };
ID3D11RenderTargetView* renderTargetView{ nullptr };

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmdShow)
{

	HWND GameWin = FindWindow(NULL, L"AssaultCube");

	if (GameWin == NULL)
	{
		MessageBox(
			NULL,                             
			L" PLS OPEN THE GAME FIRST ", 
			L"AssaultCube ESP Hack",                 
			MB_OK | MB_ICONINFORMATION      
		);
		return 0;
	}

	RECT rectGameWin{};
	
	if (GetClientRect(GameWin, &rectGameWin))
	{
		Gamewidth = rectGameWin.right - rectGameWin.left;
		Gameheight = rectGameWin.bottom - rectGameWin.top;

	}
	

	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = winProc;
	wc.hInstance = instance;
	wc.lpszClassName = L"overlay class";

	RegisterClassExW(&wc);

	window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"esp overlay",
		WS_POPUP,
		0,
		0,
		Gamewidth,
		Gameheight,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT clientArea{};
		GetClientRect(window, &clientArea);

		RECT windowArea{};
		GetWindowRect(window, &windowArea);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			windowArea.left + (diff.x - windowArea.left),
			windowArea.top + (diff.y - windowArea.top),
			clientArea.right,
			clientArea.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}
	
	DXGI_SWAP_CHAIN_DESC sd{};

	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL lvls[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	D3D_FEATURE_LEVEL lvl{};


	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		lvls,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swapChain,
		&device,
		&lvl,
		&deviceContext
	);

	ID3D11Texture2D* backBuffer{ nullptr };
	swapChain->GetBuffer(0U, IID_PPV_ARGS(&backBuffer));

	if (backBuffer)
	{
		device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
		backBuffer->Release();
	}
	else
	{
		return 1;
	}

	ShowWindow(window, cmdShow);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, deviceContext);
	
	bool running = true;
	memory game(L"ac_client.exe");
	const auto client = game.GetModuleBaseAddr(L"ac_client.exe");

	int x{}, y{};
	DWORD exitCode{};
	
	while (running)
	{
		
		MSG msg{};

		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}

		}

		if (!running)
		{
			break;
		}
	
		GetExitCodeProcess(game.GetProcH(), &exitCode);
		if (exitCode != STILL_ACTIVE)
		{
			break;
		}
		
		ImVec2 screen{};

		int ret1 = setImguiWinSize(screen, window, x, y, Gamewidth, Gameheight, instance, cmdShow);

	
		if (!ret1)
		{
			
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::GetBackgroundDrawList()->AddText({ (screen.x / 2) - 130.f, 20.f }, ImColor(255, 255, 255), "Made By Cyrus | AKA 'dzwassim95'");
			
			const auto localPayer = game.read<uintptr_t>(client + offsets::iLocalPlayer);
			const auto vm = game.read<viewMatrix>(client + offsets::iViewMatrix);
			auto playerCount = game.read<int>(client + offsets::iEntityList + 0x8);

			if (localPayer)
			{
				const auto entity = game.read<uintptr_t>(client + offsets::iEntityList);
				auto isTeam = game.read<int>(localPayer + offsets::iTeam);

				for (int i = 0; i < playerCount; ++i)
				{
					auto player = game.read<uintptr_t>(entity + 0x4 * i);

					if (game.read<int>(player + offsets::iTeam) == isTeam)
					{
						continue;
					}

					if (game.read<int>(player + offsets::isDead))
					{
						continue;
					}

					auto headPos3D = game.read<vector3>(player + offsets::vHead);
					auto feetPos3D = game.read<vector3>(player + offsets::vFeet);
					auto health = game.read<int>(player + offsets::pHealth);
					auto name = game.readName(player + offsets::pName);

					vector3 top{};
					vector3 bottom{};

					if (w2scn(headPos3D + vector3{0.f, 0.f, 0.7f}, top, vm, screen) && w2scn(feetPos3D - vector3{ 0.f, 0.f, 0.7f }, bottom, vm, screen))
					{
						const float h = bottom.y - top.y;
						const float w = h * 0.25f;
						
						ImGui::GetBackgroundDrawList()->AddRect({top.x - w, top.y},{bottom.x + w, bottom.y}, ImColor(255,255,255), 0.f, 0, 2.f);
						ImGui::GetBackgroundDrawList()->AddLine({screen.x / 2, screen.y}, {top.x, top.y}, ImColor(255,255,255), 2.f);

						// Define the dimensions and position of the health bar
						float barWidth = 10.0f;               // Width of the health bar
						float barHeight = bottom.y - top.y;             // Maximum height of the health bar
						float healthPercentage = (float)health / 100; // Normalize health (0 to 1)

						// Calculate the current height of the health bar based on health
						float currentHeight = barHeight * healthPercentage;

						// Position the health bar next to the player (to the right of the player's position)
						ImVec2 barPosition = { top.x + w + 5.0f, top.y };

						// Set color based on health
						ImColor color(0, 255, 0); // Green for full health
						if (health < 60) {
							color = ImColor(255, 255, 0); // Yellow for medium health
						}
						if (health < 30) {
							color = ImColor(255, 0, 0); // red for low health
						}

						// Draw the filled health bar (current health)
						ImGui::GetBackgroundDrawList()->AddRectFilled(
							{ barPosition.x, barPosition.y + (barHeight - currentHeight) }, // Y position is adjusted
							{ barPosition.x + barWidth, barPosition.y + barHeight }, // Bottom right corner
							color // Color of the health bar
						);

						ImGui::GetBackgroundDrawList()->AddText({ bottom.x - w, bottom.y + 3.f }, ImColor(255, 255, 255), name.c_str());
					
					}

				}

			}
		}
		
		ImGui::Render();
		
		constexpr float color[4] = {0.f, 0.f, 0.f, 0.f};
		deviceContext->OMSetRenderTargets(1U, &renderTargetView, nullptr);
	
		deviceContext->ClearRenderTargetView(renderTargetView, color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swapChain->Present(1U, 0U);
	
	}
	
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();

	ImGui::DestroyContext();
	
	if (swapChain)
	{
		swapChain->Release();
	}
	
	if (deviceContext)
	{
		deviceContext->Release();
	}

	if (device)
	{
		device->Release();
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	if (game.GetProcH())
	{
		CloseHandle(game.GetProcH());
	}

	return 0;
}