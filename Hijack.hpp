#include <Windows.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

HWND window_handle;

namespace Hijack
{
    auto Hijack() -> bool {
        window_handle = FindWindowA(("MedalOverlayClass"), ("MedalOverlay"));

        if (!window_handle)
        {
            MessageBoxA(NULL, ("Medal Not Found"), NULL, ALERT_SYSTEM_WARNING);
            return false;
        }

        MARGINS Margin = { -1 };
        DwmExtendFrameIntoClientArea(window_handle, &Margin);
        SetWindowPos(window_handle, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
        ShowWindow(window_handle, SW_SHOW);
        return true;
    }
}