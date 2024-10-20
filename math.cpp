#include "math.h"
#include "graphics.h"
#include <iostream>

bool w2scn(const vector3& world, vector3& screen, const viewMatrix& vm, ImVec2& scn)
{

    float w = world.x * vm.data[3] + world.y * vm.data[7] + world.z * vm.data[11] + vm.data[15];

    if (w < 0.001f)
    {
        return false;
    }

    float x = world.x * vm.data[0] + world.y * vm.data[4] + world.z * vm.data[8] + vm.data[12];
    float y = world.x * vm.data[1] + world.y * vm.data[5] + world.z * vm.data[9] + vm.data[13];

    float nx = x / w;
    float ny = y / w;


    screen.x = (scn.x * 0.5f * nx) + (scn.x * 0.5f + nx);
    screen.y = -(scn.y * 0.5f * ny) + (scn.y * 0.5f + ny);

    return true;
}

int setImguiWinSize(ImVec2& scrn, const HWND& win, int& a, int& b , int& c, int& d, const HINSTANCE& insta, const INT& cmSh)
{
    HWND winH = FindWindow(NULL, L"AssaultCube");

    if (winH == NULL)
    {
        return 1;
    }

    RECT rect{};
 
    if (GetClientRect(winH, &rect))
    {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        scrn = { static_cast<float>(width), static_cast<float>(height) };

        POINT pnt{};
        ClientToScreen(winH, &pnt);
       
        if ((a != pnt.x) || (b != pnt.y) || (c != width) || (d != height))
        {   

            a = pnt.x;
            b = pnt.y;
            c = width;
            d = height;
            ReloadGraphics(insta, cmSh, a, b, c, d);

        }
        return 0;
    }
    return 1;
}