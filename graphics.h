#pragma once
#include <windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include <dwmapi.h>


INT ReloadGraphics(const HINSTANCE& inst, const INT& cshow, INT& A, INT& B, INT& C, INT& D);
