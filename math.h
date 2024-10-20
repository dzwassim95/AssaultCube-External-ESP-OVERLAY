#pragma once
#include "struct.h"
#include "imgui/imgui.h"
#include <Windows.h>
#include <dwmapi.h>


bool w2scn(const vector3& world, vector3& screen, const viewMatrix& vm, ImVec2& scn);
int setImguiWinSize(ImVec2& scrn, const HWND& win, int& a, int& b, int& c, int& d, const HINSTANCE& insta, const INT& cmSh);
