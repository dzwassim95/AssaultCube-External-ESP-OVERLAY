#include "graphics.h"

extern LRESULT CALLBACK winProc(HWND window, UINT message, WPARAM wprm, LPARAM lprm);

extern int Gamewidth;
extern int Gameheight;
extern WNDCLASSEXW wc;
extern HWND window;
extern ID3D11Device* device;
extern ID3D11DeviceContext* deviceContext;
extern IDXGISwapChain* swapChain;
extern ID3D11RenderTargetView* renderTargetView;

INT ReloadGraphics(const HINSTANCE& inst, const INT& cshow, INT& A, INT& B, INT& C, INT& D)
{
	
	
	
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();

		ImGui::DestroyContext();

		if (swapChain)
		{
			swapChain->Release();
			swapChain = nullptr;
		}

		if (deviceContext)
		{
			deviceContext->Release();
			deviceContext = nullptr;
		}

		if (device)
		{
			device->Release();
			device = nullptr;
		}

		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = nullptr;
		}
	
	MoveWindow(window, A, B, C, D, TRUE);

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

	constexpr D3D_FEATURE_LEVEL lvls[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 }; // requires +d3d10

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

	ShowWindow(window, cshow);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, deviceContext);

	return 2;
}
