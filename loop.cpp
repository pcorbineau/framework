#include "loop.hpp"
#include <Windows.h>

void Loop::run() noexcept {
    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
